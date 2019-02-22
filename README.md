# DDirWatcher

DDirWatcher is another demonstration of how `BaseLayer` can be used to create distributed systems based on the selected architectural style. It took about 2 working days (within 4-5 calendar days) to implement DDirWatcher. At least a half day was wasted in trying to get `boost::property_tree` to do the XML marshalling, so that time could have been saved if a wiser selection had been made in the beginning. After struggling with property_tree, I switched to using tinyxml2 library which was better in this case.

A usage scenario for DDirWatcher could be that a group of software developers in an office space want to be explicitly aware of who is working with which components of the system. Of course you can always check the git for committed changes in the git server. But what about those changes which haven't yet been committed? So each developer installs the DDirWatcher node as a service in the development machine. The directory with the source code is then set as the `target`. A developer server hosts the last node. The last node then creates an XML file, which is included in a dynamic web page. Every time the XML file changes, the dynamic page refreshes the content of the page, including the most recent file changes in developers' local computers. The developer server has a large display which everyone can see and take a look when having a break or coffee, to see who is working on what. Those who are working remotely, can also view the web page.

DDirWatcher implements a set of Nodes which can:

- Observe a directory for changes; [libfswatch](https://github.com/emcrisostomo/fswatch/tree/master/libfswatch) is used for this.
- When a change occurs in that directory (file is edited, deleted, created or accessed, etc.), a Package is sent to the next node.
- Nodes can be chained one after each other. Or optionally, nodes can send Packages to a central Node.
- Last or central Node then maintains an XML file about the changed files in different computers. This file could be included in a web page to display in real time the files changing in different computers.
- Last node keeps e.g. 100 latest change info items and discards the older ones. Every time a new Package with change information arrives, the XML file is regenerated.
- First node can be configured to send pings every now and then.

There are also other than domain specific differences in DDirWatcher compared to StudentPassing. In DDirWatcher, nodes do not have a GUI. Actually they do not have any kind of GUI, not even a console. The nodes are started and then just left running without any user interaction. You can check out the `main.cpp` in the source code to see how a node in DDirWatcher is created and executed. Therefore, the only input to the nodes are from a) the file system changes and b) previous node(s), if any. In order to stop a Node you need to ctrl-c to kill it, or if executed with &, use kill command from the command prompt (or your environment's task manager to do it).

In addition to boost, g3logger and nlohmann::json, this application also uses [tinyxml2](https://github.com/leethomason/tinyxml2) library for generating the XML output. See the CMakeLists.txt to check out the needed libraries. Note that in DDirWatcher, there is no library corresponding to StudentLayer, but the domain specific classes and the application related classes are in the same component, the executable.

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
input	127.0.0.1:33333
user	pentti
fileout	/Users/antti/Documents/Events.
marshal	xml
bufsize	100
```
Here application specific configuration items are the `marshal` setting, which describes the format of the output which is produced. Currently xml and json are supported. JSON was easy to add since BaseLayer already uses JSON for conversions. `bufsize` determines how many file change objects at most are buffered and then marshalled to the output file. When more than 100 objects arrive (in this example), then the older objects are purged. Note that `fileout` must end in dot (".") -- the `marshal` format is added to the end of the file when creating the file for output.

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

1. boost 1.68 or higher
2. g3logger
3. nlohmann::json
4. OHARBaseLayer
5. tinyxml2
6. libfswatch

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
All *you* need to do is to create the web service reading the XML file to show file changing awareness to developers. A simple demo of this is in the SampleData subdirectory.
