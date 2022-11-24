# iTrace-Toolkit
Data processing support application for the iTrace Infrastructure

## Important Note
Because iTrace-Toolkit handles large amounts of data, any of the steps can take a while to run. Do not close iTrace-Toolkit while the loading animation is playing

## Creating a Database
Toolkit stores all work done inside of a SQLite database, the schema of which can be found [here](https://github.com/iTrace-Dev/iTrace-Toolkit/wiki/Entity-Relationship-Diagram-for-Post-Processing-Database).

To start, select the "Create Database" button. If you have a database from a previous session made, you can also choose "Open Database".

We recommend installing a tool to help look through the data stored in the database. Our recommendation is [DB Browser for SQLite](https://sqlitebrowser.org/).

## Importing Core and Plugin Data
Toolkit requires iTrace Core and associated iTrace IDE Plugin data to perform its calculations. Toolkit provides two ways to import this data:
### Folder Import
This is the recommended way to import data into Toolkit. Select "Import Data from Folder" and then select the folder that houses your Core and Plugin data. This option will automatically loop through the folder, find all valid data files, and import them if it finds a matching pair.
### Database Import
A previously used iTrace Database can be imported, brigning in a copy of all of the data from the database. This can help consolidate data from multiple tasks, if one larger file is needed.

![Data_Imported](https://user-images.githubusercontent.com/44215406/203677053-07599bb9-da16-4bf1-8361-4ba82e020831.png)

Each session contained within the imported data will be displayed in a checkbox list. For future tasks, each session can be checked/unchecked to include/exclude the session from future calculations.

**NOTE:** It is generally recommended to only import sessions recorded on the same code base.

## Mapping Tokens
Before mapping the tokens to the Plugin data, a srcML Archive File is needed. srcML is an XML format for source code that can provide positional data, and is required to map Plugin data to the appropriate source code token.
srcML can be downloaded and installed from [here](https://www.srcml.org/). To generate the srcML Archive file, open up your terminal, navigate to the source code that the session(s) were recorded on, and execute the following command:
```
srcml --archive --position [input_files] -o [output_file]
```
After this file has been made, select the "Map Tokens" button. This will bring up a small window. On this window, select the "Browse for srcML Archive" button and choose the srcML Archive File you created earlier. Finally, select "Identify Tokens". The checkbox can be selected if token mapping has already been performed and you would like to overwrite with new data.
This step of the process usually takes a while to complete. The speed is dependent on the size of the srcML Archive file and how many sessions you are evaluating.

Checking the database now will show that the ide_plugin data should have token and syntactic context information filled out.

**NOTE:** This process can only be performed on sessions that involved source code which srcML is able to work with. See the [srcML Documentation page](https://www.srcml.org/documentation.html) for more information.

## Generating Fixations
Generating fixations can happen before mapping tokens to the plugin data, but it is generally recommended to generate fixations afterwards. Otherwise, the fixations will not have token or syntactic context information provided.

To generate the fixations, the first step is to select the "Fixation Settings" button. This will greet you with a screen that has a drop-down menu and various setting boxes. The drop-down menu allows switching between which fixation generation algorithm you would like to use. The additional boxes allow you to fine tune the algorithms for various uses.
Information on each algorithm can be found below:

[Basic Algorithm](https://link.springer.com/article/10.3758/BF03207917)

[IDT and IVT Algorithms](https://link.springer.com/article/10.3758/s13428-016-0738-9)

Fixations can be generated multiple times, and with different settings. Each fixation is mapped to a fixation_run value, allowing multiple batches of fixations to be generated at once.

## Fixation Querying
After fixations have been generated, Toolkit offers the ability to sort through the fixations and select specific subsets of them. To query the fixations, select the "Query Fixations" button. The page that appears contains many different input values, each of which is detailed below:

![Fixation_Filtering](https://user-images.githubusercontent.com/44215406/203677083-fd15e483-36f7-4c7d-9623-1818707321c2.png)

* Fixation Target
	* Which files to look for. Use commas to separate multiple files, and leave empty to search all files.
* Token Type
	* Which tokens to look for. Use commas to separate multiple tokens, and leave empty to search for all tokens.
* Duration
	* How long the fixation lasted for. Can set both the minimum and maximum acceptable value. Set maximum to -1 to have no maximum.
* Source File Line
	* The line number that the fixation was looking at. Can set both the minimum and maximum acceptable value. Set maximum to -1 to have no maximum.
* Source File Column
	* The column number that the fixation was looking at. Can set both the minimum and maximum acceptable value. Set maximum to -1 to have no maximum.
* Pupil Diameters
	* The recorded size of the user's left and right pupils. Values can range between 0 and 3.5, but other maximums and minimums can be set. The chain button toggles the lock of the left and right sliders.
* Output File Type
	* The desired type of output format.

The remaining buttons are explained below:
* Export Current Settings
	* Exports the currently selected query settings into a .sql file, runnable on any Toolkit database.
* Load SQL File and Filter
	* Loads a .sql file and queries the database with the command within. Exports the queried data into the file format specified in the Output File Type box.
* Close
	* Exits back to the main Toolkit screen.
* Output Folder
	* Allows the user to set which folder any filter output will be sent to.
* Filter
	* Filters the fixations within the database using the currently selected settings. Exports the queried data into the file format specified in the Output File type box.

All query outputs are placed into the same directory as the executable. This is planned to be changed in the future.

This is not the only way to query data from the database. Because we use SQLite format, any other program can be made to run queries and analysis on a Toolkit Database - Toolkit offers some tools to help narrow the focus of datasets for convenience, but is not required.

## Log File
Toolkit generates a log file on launch that prints information about your session. This file is contained within the executable's directory. In the event of a crash, please report the crash alongside the log file.
