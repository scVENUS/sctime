## sctime Web GUI

### Importing and deleting settings

- *Settings → Import* allows importing an existing `settings.xml` file from the
  sctime Standalone GUI.

- *Settings → Delete configuration data* allows the user to delete their saved data.
  - Note carefully that all settings and all recorded times can be deleted here —
    see the selection in the menu.
  - The data cannot be recovered.


### Recording times offline

*Settings → Settings → General → Stay offline after initialization* permanently
activates offline mode.
- In offline mode, all data (settings and times) is stored in the browser only.
- If the user clears their browser data, the sctime data will be lost as well!
- As soon as the mode is deactivated, the sctime Web GUI synchronises the data and
  reports this in the status bar.

<span id="dataexp"></span>

### Exporting offline times

Users who record all their times exclusively offline must export them for further
processing:

- *Account → Download SH files*
- Select the time period
- Select and confirm the save location

The sctime Web GUI exports a `.zip` file:
- Unpack the `.zip` file
- Process the included `zeit-YYYY-MM-DD.sh` files further
