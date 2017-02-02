# dota-replay-parser

A really bad C++ emscripten compiled port of Dotabuff's [manta](https://github.com/dotabuff/manta) replay parser. Also borrows a lot from the [Alice](https://github.com/AliceStats/Alice) Source 1 replay parser.

*Why?* For Source 1, Alice could be compiled with [Emscripten](https://github.com/kripken/emscripten) and I used that to make a browser based replay viewer on top of it. I wanted to do something similar for Source 2 but Alice has not been updated. I don't have the expertise or knowledge on Dota 2 replays or C++ to be able to write my own parser from scratch so I based everything off manta and Alice.

I also tried just compiling manta with [GopherJS](https://github.com/gopherjs/gopherjs), but the performance was slow. I don't know how to write Go and combined with my poor knowledge of the inner workings of replays, I'm clueless as to how to maybe optimize it.

## Building

Sorry, but I haven't used a build system yet. It's a miracle I got anything to compile to begin with since I'm a total noob. But I'll describe what I did at least and maybe that will be enough for someone to be able to produce a build. If I learn how to create a makefile I'll update this as well.

### Dependencies

* [Emscripten](http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html)

* [Boost 1.59.0](http://www.boost.org/users/history/version_1_59_0.html)

* [Snappy compressor/decompressor](https://github.com/google/snappy)

* [Protobuf 2.6.1 Emscripten](https://github.com/invokr/protobuf-emscripten)


First get emscripten and boost installed. For me, emscripten ended up in `/home/devilesk/emsdk_portable` and boost in `/home/devilesk/boost_1_59_0`. I have `/usr/local/include/boost` symlinked to `/home/devilesk/boost_1_59_0/boost`. Follow the [updating the sdk](https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html#updating-the-sdk) steps for emscripten.

Then download the snappy source and [build using emscripten](https://kripken.github.io/emscripten-site/docs/compiling/Building-Projects.html). Basically just `./emconfigure ./configure` then `./emmake make` in the snappy directory. I ended up with everything in `/home/devilesk/snappy` and the compiled code in `/home/devilesk/snappy/.libs`

Then download protobuf-emscripten and follow the steps in its README. I ended up with `/home/devilesk/protobuf-emscripten` and the compiled code in `/home/devilesk/protobuf-emscripten/2.6.1/src/.libs`.

### Compiling

Download this project's source.

At this point it might be good to update the protos in the protobufs directory. To do so you'll need to build and install [protobuf 2.6.1](https://github.com/google/protobuf/releases/tag/v2.6.1). Then update the `protobufs/update_proto.sh` script with the correct path to your `protoc` (for me it is `/usr/local/bin/protoc`) and path to the directory where the script is located which is where the protobufs will be downloaded to (for me it is `/home/devilesk/aegis/protobufs`).

Now to actually compile the replay parser with emscripten. Run the following commands. You'll need to change the library include paths based on where your dependencies are.

```
em++ -o parser.bc -std=c++11 parser.cpp packet_entity.cpp message_lookup.cpp huffman.cpp property_serializers.cpp string_table.cpp class_info.cpp flattened_serializers.cpp fieldpath.cpp quantizedfloat.cpp property.cpp property_decoder.cpp bitstream.cpp protobufs/demo.pb.cc protobufs/netmessages.pb.cc protobufs/networkbasetypes.pb.cc protobufs/network_connection.pb.cc protobufs/usermessages.pb.cc -I /home/devilesk/snappy -I /home/devilesk/protobuf-emscripten/2.6.1/src -I /usr/local/include -s ALLOW_MEMORY_GROWTH=1 -O3

em++ -o replayviewer.bc -std=c++11 replayviewer.cpp -I /home/devilesk/snappy -I /home/devilesk/protobuf-emscripten/2.6.1/src -I /usr/local/include -s ALLOW_MEMORY_GROWTH=1 -O3

em++ --bind -std=c++11 parser.bc replayviewer.bc -o replayviewer.js -I /home/devilesk/snappy -I /home/devilesk/protobuf-emscripten/2.6.1/src -I /usr/local/include /home/devilesk/protobuf-emscripten/2.6.1/src/.libs/libprotobuf.so.9.0.1 /home/devilesk/snappy/.libs/libsnappy.so -s ALLOW_MEMORY_GROWTH=1 -O3
```

The first two commands generate two bitcode files, parser.bc and replayviewer.bc. The last command links them together along with the other libraries and outputs the javascript library `replayviewer.js` and `replayviewer.js.mem`.

## Usage

If you start up a local server to serve the directory containing the source and compiled files (`python -m SimpleHTTPServer` if you have Python 2.7) you can go to demo.html to try the parser.

I'm currently working on using this replay parser in my [dota interactive map](https://github.com/devilesk/dota-interactive-map/tree/replayviewer/src/js/replayviewer) to create a replay viewer.