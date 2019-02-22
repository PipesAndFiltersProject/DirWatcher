# Web interface to XML file

The last node of DDirWatcher is able to produce an XML file, which can be included in a dynamic web page. 

Here you can find a sample implementation of a simple scripted html page (`index.html`), which reloads the XML file (e.g. `Events.xml`) periodically to display the changed files in a html table.

Note that you must enable reading local files from your browser's preferences. Usually this is restricted / disabled *because of security reasons*. Check out the user guide of your browser / google to see how to do this. And after testing, do remember to *put the restriction back* to keep your browser secure!

In real life situation, the index.html and Events.xml would be placed in a web server, naturally, so this browser local data reading issue would not be an item to worry about.

## Usage

1. Place the index.html file to the same directory where the Events.xml file is generated (see the last node's configuration file and the fileout setting).
2. Enable the browser's usage of local data files.
3. Launch the nodes which observe directory change events.
4. Launch the node which generates the XML file.
5. Open the index.html with your browser and see it refreshing the table of changed files, as  you make changes in the target directory in the nodes observing directory change events.

After testing, again *disable* browser using local data files setting!
