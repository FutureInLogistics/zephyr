:orphan:

..
  See
  https://docs.zephyrproject.org/latest/releases/index.html#migration-guides
  for details of what is supposed to go into this document.

.. _migration_4.5:

Migration guide to Zephyr v4.5.0 (Working Draft)
################################################

This document describes the changes required when migrating your application from Zephyr v4.4.0 to
Zephyr v4.5.0.

Any other changes (not directly related to migrating applications) can be found in
the :ref:`release notes<zephyr_4.5>`.

.. contents::
    :local:
    :depth: 2

Common
******

Build System
************

Kernel
******

Boards
******

Device Drivers and Devicetree
*****************************

.. Group contents in this section by subsystem, e.g.:
..
.. ADC
.. ===
..
.. ...

.. zephyr-keep-sorted-start re(^\w) ignorecase

Flash
=====

* The STM32 flash extended operations :c:enumerator:`FLASH_STM32_EX_OP_OPTB_WRITE` and
  :c:enumerator:`FLASH_STM32_EX_OP_RDP` no longer relaunch the option byte loader on the STM32G0,
  STM32G4, STM32L4 and STM32L5 series, so they no longer reset the device. The programmed values
  take effect at the next power-on reset, as they already did on the other series. Applications
  that relied on the implicit reset should issue :c:enumerator:`FLASH_STM32_EX_OP_OPTB_RELOAD`
  after writing.

.. zephyr-keep-sorted-stop

Bluetooth
*********


Networking
**********

Other subsystems
****************

Modules
*******

Architectures
*************
