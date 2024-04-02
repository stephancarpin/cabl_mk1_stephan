
        ##########    Copyright (C) 2015 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #######

include(sl-functions)

# ------------------------------------------------------------------------------------------------ #
#  RtMidi                                                                                          #
# ------------------------------------------------------------------------------------------------ #
function (addRtMidi)
  if(DEFINED RTMIDI_INCLUDE_DIR)
    message(STATUS "RtMidi is already available")
  else()
    checkout_external_project(rtmidi https://github.com/thestk/rtmidi.git master)
    set(RTMIDI_INCLUDE_DIR ${CMAKE_BINARY_DIR}/rtmidi/src/rtmidi PARENT_SCOPE)
    set(RTMIDI_INCLUDE_DIR ${CMAKE_BINARY_DIR}/rtmidi/src/rtmidi)
    message(STATUS "RtMidi path: ${RTMIDI_INCLUDE_DIR}")

    set(
      lib_rtmidi_LIBRARY
        ${RTMIDI_INCLUDE_DIR}/RtMidi.h
        ${RTMIDI_INCLUDE_DIR}/RtMidi.cpp
    )
    source_group("src"  FILES  ${lib_rtmidi_LIBRARY})
    add_library( rtmidi STATIC ${lib_rtmidi_LIBRARY})

    set_target_properties(
      rtmidi
      PROPERTIES
        OUTPUT_NAME         "rtmidi"
        OUTPUT_NAME_DEBUG   "rtmidi${DEBUG_SUFFIX}"
    )

    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(
      rtmidi
      PROPERTIES
        COMPILE_DEFINITIONS __MACOSX_CORE__
    )
    endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  endif()
endfunction()


# ------------------------------------------------------------------------------------------------ #
#  Catch                                                                                           #
# ------------------------------------------------------------------------------------------------ #
function (addCatch)
  if(DEFINED CATCH_INCLUDE_DIR)
    message(STATUS "Catch is already available")
  else()
    checkout_external_project(catch https://github.com/philsquared/Catch.git master)
    set(CATCH_INCLUDE_DIR ${CMAKE_BINARY_DIR}/catch/src/catch/include PARENT_SCOPE)
    set(CATCH_INCLUDE_DIR ${CMAKE_BINARY_DIR}/catch/src/catch/include)
    message(STATUS "Catch path: ${CATCH_INCLUDE_DIR}")
  endif()
endfunction()


# ------------------------------------------------------------------------------------------------ #
#  LibUSB                                                                                          #
# ------------------------------------------------------------------------------------------------ #
function (addLibUSB)
  if(DEFINED LIBUSB_INCLUDE_DIR)
    message(STATUS "libUSB is already available")
  else()
    checkout_external_project(libusb https://github.com/libusb/libusb.git master)
    set(LIBUSB_BASE_DIR ${CMAKE_BINARY_DIR}/libusb/src/libusb/)
    set(LIBUSB_INCLUDE_DIRS ${LIBUSB_BASE_DIR}/libusb ${LIBUSB_BASE_DIR}/libusb/os PARENT_SCOPE)
    set(LIBUSB_INCLUDE_DIRS ${LIBUSB_BASE_DIR}/libusb ${LIBUSB_BASE_DIR}/libusb/os)

    message(STATUS "libUSB path: ${LIBUSB_BASE_DIR}")

    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
      set(
        lib_libusb_LIBRARY
          ${LIBUSB_BASE_DIR}/msvc/config.h
          ${LIBUSB_BASE_DIR}/libusb/core.c
          ${LIBUSB_BASE_DIR}/libusb/descriptor.c
          ${LIBUSB_BASE_DIR}/libusb/hotplug.c
          ${LIBUSB_BASE_DIR}/libusb/io.c
          ${LIBUSB_BASE_DIR}/libusb/os/poll_windows.c
          ${LIBUSB_BASE_DIR}/libusb/strerror.c
          ${LIBUSB_BASE_DIR}/libusb/sync.c
          ${LIBUSB_BASE_DIR}/libusb/os/threads_windows.c
          ${LIBUSB_BASE_DIR}/libusb/os/windows_usb.c
      )
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
      set(
        lib_libusb_LIBRARY
          ${LIBUSB_BASE_DIR}/msvc/config.h
          ${LIBUSB_BASE_DIR}/libusb/core.c
          ${LIBUSB_BASE_DIR}/libusb/descriptor.c
          ${LIBUSB_BASE_DIR}/libusb/hotplug.c
          ${LIBUSB_BASE_DIR}/libusb/io.c
          ${LIBUSB_BASE_DIR}/libusb/os/poll_posix.c
          ${LIBUSB_BASE_DIR}/libusb/strerror.c
          ${LIBUSB_BASE_DIR}/libusb/sync.c
          ${LIBUSB_BASE_DIR}/libusb/os/threads_posix.c
      )
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      set(
        lib_libusb_LIBRARY
          ${LIBUSB_BASE_DIR}/msvc/config.h
          ${LIBUSB_BASE_DIR}/libusb/core.c
          ${LIBUSB_BASE_DIR}/libusb/descriptor.c
          ${LIBUSB_BASE_DIR}/libusb/hotplug.c
          ${LIBUSB_BASE_DIR}/libusb/io.c
          ${LIBUSB_BASE_DIR}/libusb/os/poll_posix.c
          ${LIBUSB_BASE_DIR}/libusb/strerror.c
          ${LIBUSB_BASE_DIR}/libusb/sync.c
          ${LIBUSB_BASE_DIR}/libusb/os/threads_posix.c
          ${LIBUSB_BASE_DIR}/libusb/os/darwin_usb.c
      )
    endif()
    source_group("src" FILES  ${lib_libusb_LIBRARY})
    add_library(libusb STATIC ${lib_libusb_LIBRARY})
    set_target_properties(
      libusb
      PROPERTIES
        OUTPUT_NAME         "libusb"
        OUTPUT_NAME_DEBUG   "libusb${DEBUG_SUFFIX}"
    )

    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
      target_include_directories(libusb PUBLIC ${LIBUSB_INCLUDE_DIRS} ${LIBUSB_BASE_DIR}/msvc)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      target_include_directories(libusb PUBLIC ${LIBUSB_INCLUDE_DIRS} ${LIBUSB_BASE_DIR}/Xcode)
    endif()

  endif()
endfunction()

# ------------------------------------------------------------------------------------------------ #
#  HIDAPI                                                                                          #
# ------------------------------------------------------------------------------------------------ #
function (addHIDAPI)
  if(DEFINED HIDAPI_INCLUDE_DIR)
    message(STATUS "HIDAPI is already available")
  else()
    checkout_external_project(hidapi https://github.com/signal11/hidapi.git master)
    set(HIDAPI_BASE_DIR ${CMAKE_BINARY_DIR}/hidapi/src/hidapi/)
    set(HIDAPI_INCLUDE_DIR ${CMAKE_BINARY_DIR}/hidapi/src/hidapi/hidapi PARENT_SCOPE)
    set(HIDAPI_INCLUDE_DIR ${CMAKE_BINARY_DIR}/hidapi/src/hidapi/hidapi)

    message(STATUS "HIDAPI path: ${HIDAPI_BASE_DIR}")

    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
      set(
        lib_hidapi_LIBRARY
          ${HIDAPI_BASE_DIR}/hidapi/hidapi.h
          ${HIDAPI_BASE_DIR}/windows/hid.c
      )
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
      set(
        lib_hidapi_LIBRARY
        ${HIDAPI_BASE_DIR}/hidapi/hidapi.h
        ${HIDAPI_BASE_DIR}/libusb/hid.c
      )
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      set(
        lib_hidapi_LIBRARY
        ${HIDAPI_BASE_DIR}/hidapi/hidapi.h
        ${HIDAPI_BASE_DIR}/mac/hid.c
      )
    endif()

    source_group("src" FILES  ${lib_hidapi_LIBRARY})
    add_library(hidapi STATIC ${lib_hidapi_LIBRARY})

    set_target_properties(
      hidapi
      PROPERTIES
        OUTPUT_NAME         "hidapi"
        OUTPUT_NAME_DEBUG   "hidapi${DEBUG_SUFFIX}"
    )
    target_include_directories(hidapi PUBLIC ${HIDAPI_INCLUDE_DIR})

  endif()
endfunction()
