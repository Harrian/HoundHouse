/* file "SimpleAudioQueriesOnlyWrittenResponse.cpp" */

#include "PhraseSpotterAPI.h"
#include "HoundCloudRequester.h"
#include "HoundConverser.h"
#include "ClientCapabilityRegistry.h"
#include "PlatformSpecificClientCapabilities.h"
#include "throwf.h"

#include "SimpleHoundCloudRequesterConfig.h"
#include "OkHoundSink.h"
#include "SimpleRequestInfoPreparer.h"
#include "SimplePartialHandler.h"
#include "SimpleSinks.h"
#include "SimpleDynamicResponseHandlers.h"

extern "C"
{
#include "o_integer.h"
}

#include <iostream>
#include <cstring>
#ifdef RESPEAKERLEDRING
#include <cstdint>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <omp.h>
#endif /* RESPEAKERLEDRING */


static void dump_usage(void);

static void process_request(
  HoundConverser * converser,
  ClientCapabilityRegistry::AudioSource * audio_device,
  SimplePartialHandler * local_handler);

static void dump_usage(const char * const prog_name){
  std::cout << "Usage: "
            << prog_name
            << " "
            << "[--lat LATITUDE]"
            << " "
            << "[--long LONGITUDE]"
            << " "
            << "ALSARECORDINGDEVICE"
            << std::endl;

  std::cout << "ALSARECORDINGDEVICE should be a device name from `arecord -L` prefixed"
            << std::endl
            << "with 'alsa:' ex: alsa:plughw:CARD=PCH,DEV=0"
            << std::endl;

  return;
}

int main(int argc, char** argv){

  if(argc<2||argc>6){
    dump_usage(*(argv));

    return 1;
  }

  bool passed_lat = false;
  double lat = 0.0;
  bool passed_lon = false;
  double lon = 0.0;

  unsigned int positional_arg_counter = 0;
  unsigned int alsa_recording_device_index = 0;

  for(int i=1;i<argc;i++){
    if(std::strcmp("--lat",*(argv+i))==0){
      if(i==argc-1){
        std::cerr << "Error: --lat requires one argument"
                  << std::endl;
        return 1;
      } else {
        char * next_value = *(argv+i+1);
        char * end_ptr = NULL;
        lat = std::strtod(next_value, &end_ptr);
        if(next_value == end_ptr){
          std::cerr << "Error: argument passed to --lat must be a double"
                    << std::endl;
          return 1;
        } else {
          passed_lat = true;
          i++;
        }
      }
    } else if(std::strcmp("--long",*(argv+i))==0) {
      if(i==argc-1){
        std::cerr << "Error: --long requires one argument"
                  << std::endl;
        return 1;
      } else {
        char * next_value = *(argv+i+1);
        char * end_ptr = NULL;
        lon = std::strtod(next_value, &end_ptr);
        if(next_value == end_ptr){
          std::cerr << "Error: argument passed to --long must be a double"
                    << std::endl;
          return 1;
        } else {
          passed_lon = true;
          i++;
        }
      }
    } else {
      if(positional_arg_counter > 0){
        std::cerr << "Error: too many positional arguments passed" <<std::endl;
        return 1;
      }else if(positional_arg_counter == 0){
        if(std::strncmp("alsa:",*(argv+i),5)!=0){
          std::cerr << "ALSARECORDINGDEVICE should be a device name from `arecord -L` prefixed"
                    << std::endl
                    << "with 'alsa:' ex: alsa:plughw:CARD=PCH,DEV=0"
                    << std::endl;
          return 1;
        } else {
          positional_arg_counter++;
          alsa_recording_device_index = i;
        }
      }
    }
  }

  if(positional_arg_counter != 1){
    dump_usage(*(argv));

    return 1;
  }

#ifdef HOUNDHOUSEDEBUG
  std::cout<<"Running with debug messages compiled in."<<std::endl;
#endif /* HOUNDHOUSEDEBUG */

  init_o_integer_module();

  PhraseSpotterSetThreshold(0.4);

#ifdef RESPEAKERLEDRING
  wiringPiSetupGpio();

  pinMode(5,OUTPUT);

  int gpio_fd;
  if((gpio_fd = wiringPiSPISetup(0, 8000000)) < 0){
    std::cerr << "wiringPiSPISetup failed" << std::endl;
  }

  std::uint8_t r, g, b, brightness;
  r = 0x00;
  g = 0x00;
  b = 0xFF;
  brightness = 1;
  std::uint8_t led_frame[48];
  std::uint8_t begin_empty_frame[4] = {0x00, 0x00, 0x00, 0x00};
  std::uint8_t close_empty_frame[1] = {0x00};
  for(int j=0;j<12;j++){
    led_frame[(j*4)] = 0b11100000 | (0b00011111 & brightness);
    led_frame[(j*4)+1] = b;
    led_frame[(j*4)+2] = g;
    led_frame[(j*4)+3] = r;
  }

  brightness = 11;

  led_frame[36] = 0b11100000 | (0b00011111 & brightness);

  brightness = 21;

  led_frame[40] = 0b11100000 | (0b00011111 & brightness);

  brightness = 31;

  led_frame[44] = 0b11100000 | (0b00011111 & brightness);
#endif /* RESPEAKERLEDRING */

  HoundCloudRequester requester(
    simple_config.clientId,
    simple_config.clientKey,
    simple_config.userId
  );

  static SimplePartialHandler local_handler;
  static SimpleRequestInfoPreparer simple_request_info_preparer(passed_lat,lat,passed_lon,lon);
  static SimpleDynamicResponseHandlerWithNoAudioOutput
    simple_dynamic_response_handler_with_no_audio_output(&local_handler);

  HoundConverser converser(&requester);
  converser.register_partial_handler(&local_handler);
  converser.register_request_info_preparer(
    &simple_request_info_preparer
  );
  converser.register_dynamic_response_handler(
    &simple_dynamic_response_handler_with_no_audio_output
  );

  ClientCapabilityRegistry capability_registry;
  do_platform_specific_client_capability_registration(&capability_registry);
  try{
    ClientCapabilityRegistry::AudioSource * audio_source_to_use =
      capability_registry.lookup_audio_source(*(argv+alsa_recording_device_index));
    if (audio_source_to_use == NULL) {
      throwf("Specified using audio input `%s', but no audio "
             "input by that name is known.\n", *(argv+alsa_recording_device_index));
    }
    while(true){
      OkHoundSink ok_hound_sink;
      audio_source_to_use->capture(16000, 1, 16, true, &ok_hound_sink);
#ifdef RESPEAKERLEDRING
      digitalWrite(5,HIGH);
      #pragma omp parallel num_threads(2)
      {
        if(omp_get_thread_num()==0){
          while(digitalRead(5)){
            write(gpio_fd,begin_empty_frame,4);
            write(gpio_fd,led_frame,48);
            write(gpio_fd,close_empty_frame,1);
            for(int i=0;i<11;i++)
              std::swap(led_frame[i*4],led_frame[(i+1)*4]);
            usleep(75000);
          }
        } else if(omp_get_thread_num()==1){
          process_request(&converser, audio_source_to_use, &local_handler);
        }
      }
#else /* RESPEAKERLEDRING */
      process_request(&converser, audio_source_to_use, &local_handler);
#endif
    }
  }
  catch (char *e1){
    std::cerr<<"Error: "<<e1<<std::endl;
    free(e1);
    return 1;
  }
  catch (const char *e1)
  {
    std::cerr<<"Error: "<<e1<<std::endl;
    return 1;
  }

  cleanup_o_integer_module();
  return 0;
}

static void process_request(
  HoundConverser * converser,
  ClientCapabilityRegistry::AudioSource * audio_device,
  SimplePartialHandler * local_handler)
{
  HoundConverser::VoiceRequest *voice_request =
    converser->start_voice_request();

  local_handler->beginQuery();

#ifdef RESPEAKERLEDRING
  SimpleSinkTurnOffLight sink(voice_request, local_handler);
#else /* RESPEAKERLEDRING */
  SimpleSinkWithoutClosingSound sink(voice_request, local_handler);
#endif
  audio_device->capture(16000, 1, 16, true, &sink);

  voice_request->finish();
  delete voice_request;
}
