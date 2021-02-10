# manch_decode

Manchester code (also known as phase encoding, or PE) is a line code in which the encoding of each data bit is either low then high, or high then low, for equal time. It is a self-clocking signal with no DC component. As a result, electrical connections using a Manchester code are easily galvanically isolated. Manchester coding is a special case of binary phase-shift keying (BPSK), where the data controls the phase of a square wave carrier whose frequency is the data rate. Manchester code ensures frequent line voltage transitions, directly proportional to the clock rate; this helps clock recovery.

 This arduino sketch provides software to decode manchester encoding.
 The code is abstract and can be used independently of the hardware used.
 You just need to provide the logic levels and the interval between transitions to the decode function.
 
 The decoding runs assyncronously and fills a buffer to be used later, using the functions *avaliable()* and *read()*.
 The buffer size is 8 bytes but there' no hard limit, you can change it from 1 to watever you want.
 
 No hardware interrupt is required, but you can capture transisions using one if you want.
 
 You need to send a `11110100` byte before your data stream.
 The `1111` part is used as a PREAMBLE and needed to calculate the correct timing used,
 the first `0` bit is used as a `SYNC` signal and a checksum is performed on `0100`,
 to check it the `PREAMBLE` and `SYNC` occurred on the correct timing. If the CHECKSUM is passed then the next bytes are decoded and pushed to the buffer.
 If the timing is off or `SYNC` is lost, or checksum is invalid, it will set the `RESYNC` flag and wait for PREABMLE-CHECK byte (`0b11110100`) to start decoding again.
 This avoids the buffer being filled with garbage.
 The streams is MSB-First (most significant bit first).
                                           
 The preamble can be as long as you want eg:
 <pre>
                   checksum (0100)
 ....preamble........VVVV
 111111111111111111110100
                    /\
                   sync (first transision)
</pre>

This is meant to be a simple implementation, it is NOT a full communication protocol, this sits at a layer below the protocol and it is only responsible for the data decoding.
Even tho you could it as is, a handshakes and error correction (CRC) should be implemented on top of this.
Your protocol can set `Manch.status = RESYNC` to force a resync (in case of CRC Errors), the sender should restart the transmission, sending a preamble and handshake again.

----

#### Manch Encode

A real world scenario of the usage of this code is provided. You can use a arduino with an light detector (LDR) to decode information transmitted through a flashing box on the screen of a computer or mobile device web browser.

http://victornpb.github.io/manch_decode/manch_encode.js/manch_encode.html

\* *Tested on WebKit Browsers*
