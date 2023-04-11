## OMNYESPQ

A full-featured implementation of Producer/Consumer pattern using
`queues` provided by `FreeRTOS`.

## Requiremenets and Implementation

For sake of simplycity the `FreeRTOS's` timers were used to produce incremented
value of integer counter. On the other side, the `main` thread prints
all the received values, which is perfectly correct implementation.

## Extra Features.

To demonstrate atleast basic capabilities of the pattern two types of
messages were used.

## Installation

The installation does not differ from any other `esp32` application,
as usual, you have to have `esp32-idf` installed, then check the
`git` repository out:

```sh
git clone https://github.com/makimy/omnyespq.git
```

Compile and flash it:

```sh
cd omnyespq
idf.py flash monitor
```

Note the `monitor` option which is used to demonstrate program's output.

## License

 IDC - I don't care.
