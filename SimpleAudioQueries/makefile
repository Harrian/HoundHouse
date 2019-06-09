all: rpi

CPP=g++
CPP_FLAGS=
LINK_CPP=g++
LINK_FLAGS= -Wl,--export-dynamic

#DEFINE_FEATURE_FLAGS=
DEFINE_FEATURE_FLAGS= -DWITHAUDIOOUTPUTFEATURES
#DEFINE_FEATURE_FLAGS= -DWITHAUDIOOUTPUTFEATURES -DWITHALLSMARTDEVICESUPPORT

DEFINE_DEBUG_FLAGS=
#DEFINE_DEBUG_FLAGS= -DHOUNDHOUSEDEBUG

PLATFORM_SPECIFIC_INCLUDES=
PLATFORM_SPECIFIC_LIBS_PATHS=
PLATFORM_SPECIFIC_LIBS=

linux: PLATFORM_SPECIFIC_INCLUDES=-I../OkHound/linux-x86_64
linux: PLATFORM_SPECIFIC_LIBS_PATHS=-L../OkHound/linux-x86_64
linux: PLATFORM_SPECIFIC_LIBS=-lPhraseSpotter
linux: SimpleAudioQueries.out

rpi: PLATFORM_SPECIFIC_INCLUDES=-I../OkHound/raspberrypi
rpi: PLATFORM_SPECIFIC_LIBS_PATHS=-L../OkHound/raspberrypi
rpi: PLATFORM_SPECIFIC_LIBS=-lPhraseSpotter
rpi: CPP_FLAGS+= -Os -Wl,--no-keep-memory -Wl,--reduce-memory-overhead
rpi: LINK_FLAGS+= -Os -Wl,--no-keep-memory -Wl,--reduce-memory-overhead
#rpi: DEFINE_FEATURE_FLAGS+= -DRESPEAKERLEDRING
rpi: SimpleAudioQueries.out

SimpleAudioQueries.o LocalConfig.o: %.o: %.cpp
	$(CPP) $(CPP_FLAGS) $(DEFINE_FEATURE_FLAGS) $(DEFINE_DEBUG_FLAGS) -c $< -I../../HoundCpp/c_foundations -I../../HoundCpp/ReferenceCounting -I../../HoundCpp/SharedArray -I../../HoundCpp/Common -I../../HoundCpp/Numbers -I../../HoundCpp/SalmonEye -I../../HoundCpp/Utilities -I../../HoundCpp/GoldenRetriever -I../../HoundCpp/JSON -I../../HoundCpp/HoundJSON -I../../HoundCpp/HoundRequester -I../../HoundCpp/HoundConverser -I../../HoundCpp/ClientCapabilityRegistry -I../../HoundCpp -I../HoundifyExplorerUtils $(PLATFORM_SPECIFIC_INCLUDES) -I../SimpleHeaders -I../ $(if $(findstring -DWITHALLSMARTDEVICESUPPORT,$(DEFINE_FEATURE_FLAGS)),-I../SmartDevices/include/) $(if $(findstring -DRESPEAKERLEDRING,$(DEFINE_FEATURE_FLAGS)),-fopenmp)

PlatformSpecificClientCapabilitiesLinux.o HoundClientALSAAudio.o WAVAudioDecoder.o: %.o: ../HoundifyExplorerUtils/%.cpp
	$(CPP) $(CPP_FLAGS) $(DEFINE_DEBUG_FLAGS) -c $< -I../../HoundCpp/c_foundations -I../../HoundCpp/ReferenceCounting -I../../HoundCpp/SharedArray -I../../HoundCpp/Common -I../../HoundCpp/Numbers -I../../HoundCpp/SalmonEye -I../../HoundCpp/Utilities -I../../HoundCpp/GoldenRetriever -I../../HoundCpp/JSON -I../../HoundCpp/HoundJSON -I../../HoundCpp/HoundRequester -I../../HoundCpp/HoundConverser -I../../HoundCpp/ClientCapabilityRegistry -I../../HoundCpp -I../HoundifyExplorerUtils

SimpleDataSourceFromBase64EncodedString.o: %.o: ../SimpleHeaders/%.cpp
	$(CPP) $(CPP_FLAGS) $(DEFINE_DEBUG_FLAGS) -c $< -I../../HoundCpp/c_foundations -I../../HoundCpp/ReferenceCounting -I../../HoundCpp/SharedArray -I../../HoundCpp/Common -I../../HoundCpp/Numbers -I../../HoundCpp/SalmonEye -I../../HoundCpp/Utilities -I../../HoundCpp/GoldenRetriever -I../../HoundCpp/JSON -I../../HoundCpp/HoundJSON -I../../HoundCpp/HoundRequester -I../../HoundCpp/HoundConverser -I../../HoundCpp/ClientCapabilityRegistry -I../../HoundCpp -I../HoundifyExplorerUtils

SimpleAudioQueries.out: SimpleAudioQueries.o LocalConfig.o PlatformSpecificClientCapabilitiesLinux.o HoundClientALSAAudio.o WAVAudioDecoder.o $(if $(findstring -DWITHAUDIOOUTPUTFEATURES,$(DEFINE_FEATURE_FLAGS)), SimpleDataSourceFromBase64EncodedString.o)
	$(LINK_CPP) $(LINK_FLAGS) -o SimpleAudioQueries.out SimpleAudioQueries.o LocalConfig.o PlatformSpecificClientCapabilitiesLinux.o HoundClientALSAAudio.o WAVAudioDecoder.o $(if $(findstring -DWITHAUDIOOUTPUTFEATURES,$(DEFINE_FEATURE_FLAGS)), SimpleDataSourceFromBase64EncodedString.o) -L../../HoundCpp/c_foundations -L../../HoundCpp/ReferenceCounting -L../../HoundCpp/SharedArray -L../../HoundCpp/Common -L../../HoundCpp/Numbers -L../../HoundCpp/SalmonEye -L../../HoundCpp/Utilities -L../../HoundCpp/GoldenRetriever -L../../HoundCpp/JSON -L../../HoundCpp/HoundJSON -L../../HoundCpp/HoundRequester -L../../HoundCpp/HoundConverser -L../../HoundCpp/ClientCapabilityRegistry $(PLATFORM_SPECIFIC_LIBS_PATHS) $(if $(findstring -DWITHALLSMARTDEVICESUPPORT,$(DEFINE_FEATURE_FLAGS)),-L../SmartDevices/lib) -lclientcapabilityregistry -lhoundconverser -lhoundrequester -lhoundjson -ljson_library -lGoldenRetriever -lutilities -lSalmonEye -lnumbers -lc_foundations -lasound -lncurses -lpthread -ldl -lssl -lcrypto $(PLATFORM_SPECIFIC_LIBS) $(if $(findstring -DRESPEAKERLEDRING,$(DEFINE_FEATURE_FLAGS)),-lwiringPi -lwiringPiDev -lm -lcrypt -lrt -lgomp -fopenmp) $(if $(findstring -DWITHALLSMARTDEVICESUPPORT,$(DEFINE_FEATURE_FLAGS)),-lSmartDevices)

clean:
	-rm -f SimpleAudioQueries.out SimpleAudioQueries.o LocalConfig.o PlatformSpecificClientCapabilitiesLinux.o HoundClientALSAAudio.o WAVAudioDecoder.o SimpleDataSourceFromBase64EncodedString.o
