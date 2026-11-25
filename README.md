# ExperimentProject
This is an experiment in TC3xx family of Infineon Micro controller.

This project has multi core environment.

This project reads the ADC to convert into voltage and figure out whether the read voltage is in range or abnormal. Setting faults ..etc is not in scope.

It toggles multiple LEDs in a rythm by using random number of 2 bytes. 

Also it contains the data push and pot into a circular buffer using pointers.

As part of this project scope, everything been called from core0. Other cores just runs.
