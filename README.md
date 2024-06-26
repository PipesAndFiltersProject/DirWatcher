# DDirWatcher

## Background

DDirWatcher is a demonstration of how `ProcessorNode` component can be used to create distributed systems based on the selected architectural style. 

It took only about 2 work days (within 4-5 calendar days) to implement DDirWatcher on top of ProcessorNode. Obviously knowing ProcessorNode helped in the speedy development.


## Usage scenario

A group of software developers in an office space want to be explicitly aware of who is working with which components (source code files) of the system. Of course you can always check the git for committed changes in the git server. But what about those changes which haven't yet been committed and pushed to central repository? 

Each developer installs the DDirWatcher node in the development machine. The directory with the source code is then set as the `target` to watch. A developer server in the premises hosts the last node. The last node then creates an XML file containing information about the changed files in different developer machines. This XML file is parsed by a dynamic web page. 

The dynamic page periodically refreshes the content of the page, including the most recent file changes in developers' local computers. The developer server has a large display/projector, which everyone can see. Thus, when having a break or coffee, developers see who is working on what. Those who are working remotely, can also view the web page, if accessible from outside.

## Architecture

DDirWatcher implements a set of Nodes which can:

- Observe a directory for changes; [libfswatch](https://github.com/emcrisostomo/fswatch/tree/master/libfswatch) is used for this.
- When a change occurs in that directory (file is edited, deleted, created or accessed, etc.), a Package is sent to the next node.
- Nodes can be chained one after each other. Or optionally, nodes can send Packages directly to the central Node on the local developer server.
- Last or central Node then maintains an XML file about the changed files in different computers. This file could be included in a web page to display in real time the files changing in different computers.
- Last node keeps e.g. 100 latest change info items and discards the older ones. Every time a new Package with change information arrives, the XML file is regenerated.

DDirWatcher nodes do not have a GUI. The nodes are started and then just left running without any user interaction. You can check out the `main.cpp` in the source code to see how a node in DDirWatcher is created and executed. Therefore, the only input to the nodes are from a) the file system changes and b) previous node(s), if any. In order to stop a Node you need to ctrl-c to kill it, or if executed with &, use kill command from the command prompt (or your environment's task manager to do it).

In addition to Boost, g3logger and nlohmann::json, this application also uses [tinyxml2](https://github.com/leethomason/tinyxml2) library for generating the XML output. nlohmann::json is used to create the json output file. See the CMakeLists.txt to check out the needed libraries.

The domain specific classes and the application related classes are:

* The `DDirWatcherDataItem` class is the application specific data object, extending `DataItem` from ProcessorNode library.
* In the Leaf Nodes, the `DDirWatcherHandler` does the actual file system monitoring and produces file system change events as data item objects,,to be send ahead.
* In the Leaf Nodes, the `DDirWatcherOutputHandler` provides the converts the data items to JSON and puts it into the package to be sent ahead to LastNode.
* In the Last Node, the `DDirWatcherInputHandler` parses app specific payload from incoming JSON package, and finally,
* in the Last Node, the `DDirWatcherMarshallerHandler` marshalls (or exports) the received data items as either XML or JSON, depending on configuration, to be used by a web app showing the file system events to users (XML only).

## Example configurations

The example configuration here has only two configurations. First node, which observes a directory and optionally sends repeating ping messages, and Last node, which does not observe a directory but collects file system change events from First nodes and reports them into an XML file.

In the usage scenario described above, each developer machine would host a First node and the developer server would host the Last node.

First node configuration file looks like this:

```
nodeconfiguration
input	null
output	127.0.0.1:33333
user	antti
target	/Users/antti/Downloads
ping	60
```

`input` and `output` configs are as usual in ProcessorNode. `user` identifies the user who is running the system. `target` is the directory which is observed. `ping` value (60) configures the node to send the ping message every 60 seconds.

Last node configuration example file looks like this:

```
nodeconfiguration
input	33333
user	pentti
fileout	/Users/antti/Documents/Events
marshal	xml
bufsize	100
```
Here application specific configuration items are the `marshal` setting, which describes the format of the output which is produced. Currently xml and json are supported. JSON was easy to add since BaseLayer already uses JSON for conversions. `bufsize` determines how many file change objects at most are buffered and then marshalled to the output file. When more than 100 objects arrive (in this example), then the older objects are purged. Note that  `fileout` should not end in dot (".") -- the dot and  `marshal` format (xml or json) is added to the end of the file when creating the file for output.

This is an example of the xml file produced  by the last node from a real run of the system:

```
<targets>
  <target name="/Users/antti/Downloads/DigitalClock/Readme.txt">
    <users>
      <user>antti</user>
    </users>
    <events>
      <event type="Renamed">4</event>
      <event type="AttributeModified">4</event>
      <event type="IsFile">4</event>
    </events>
  </target>
  <target name="/Users/antti/Downloads/DigitalClock/main.cpp">
    <users>
      <user>antti</user>
    </users>
    <events>
      <event type="IsFile">1</event>
    </events>
  </target>
  <target name="/Users/antti/Downloads/DigitalClock/main copy.cpp">
    <users>
      <user>antti</user>
    </users>
    <events>
      <event type="IsFile">3</event>
      <event type="Created">1</event>
      <event type="PlatformSpecific">2</event>
      <event type="OwnerModified">1</event>
      <event type="IsFile">1</event>
      <event type="Updated">1</event>
      <event type="Renamed">1</event>
    </events>
  </target>
</targets>
```
See also the SampleData subdirectory in the project. It contains an index.html page which parses Events.xml using javascript to display a table of file change events. See readme file in SampleData for details.

## Building

First make sure all the external components have been installed:

1. boost 1.70 or higher
2. g3logger
3. nlohmann::json
4. ProcessorNode (requires C++17)
5. tinyxml2
6. libfswatch

Copy `tinyxml2.cpp` and `tinyxml2.h` into this project directory.

Then in DDirWatcher:

1. mkdir build
2. cd build
3. cmake ..
4. make

Then launch the nodes. Using the above example configuration, in developer machines, configure the relevant items in the .cfg files and then run the nodes:

```
./DirWatchService ../FirstNode.cfg
```
And in the developer server, after configuring the .cfg file:

```
./DirWatchService ../LastNode.cfg
```
All *you* need to do is to create the web service reading the XML file to show file changing awareness to developers. A simple demo html page of this is in the SampleData subdirectory.


## Who do I talk to?

Repo is owned and maintained by Antti Juustila (antti.juustila at oulu.fi)

## License

(c) Antti Juustila 2014-2020. This software is licensed in [GPL version 2 or later](https://opensource.org/licenses/gpl-2.0.php).

