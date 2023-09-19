# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(motp_testing)

if((NOT CONFIG_DISK_DRIVER_FLASH) AND
   (NOT CONFIG_DISK_DRIVER_RAM) AND
   (NOT CONFIG_DISK_DRIVER_SDMMC))
	message( FATAL_ERROR "No disk access settings detected." )
endif()

FILE(GLOB app_sources src/*.c)
if (CONFIG_BT_ISO_UNICAST)
  FILE(GLOB iso_sources src/iso/*.c)
endif()
if (CONFIG_BT_AUDIO)
  FILE(GLOB audio_sources src/audio/*.c)
endif()
target_sources(app PRIVATE ${app_sources} ${iso_sources} ${audio_sources})
