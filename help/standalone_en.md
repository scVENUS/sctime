# sctime Standalone GUI

1. [Logfile](#log)
1. [Data storage](#data)
1. [Local lock](#sperre)
1. [Fonts](#font)
1. [Shortcuts](#shortcuts)
1. [Notes for Windows](#win)
1. [Notes for Mac](#ios)
1. [Data sources (power users)](#datasources)
1. [Partial offline usage](#offline)
1. [Portable edition](#portabel)


<span id="log"></span>

## Logfile

When sctime is started with the option `--logfile=FILENAME`, system messages are
written to a file and can thus be evaluated even after a crash, for example.


<span id="data"></span>

## Data storage: location and format

The program saves the data it generates (times and settings) in the subdirectory
`.sctime` of the user's home directory. Times are stored both as shell scripts
and as XML files.

All settings are saved in `settings.xml`. The program creates a backup copy of the
state before the last launch: `settings.xml.bak`. The list of personal accounts is
saved in both `settings.xml` and `zeit-YYYY-MM-DD.xml`.

The encoding of the `zeit-YYYY-MM-DD.sh` files is always UTF-8.

<span id="sperre"></span>

## Local lock

As soon as the program starts, it attempts to acquire an exclusive local lock.
The operating system reliably removes this local lock after a potential program crash
(on UNIX: `lockf()`). Once the local lock has been acquired, the program checks for
the existence of the file `.sctime/LOCK`. If it exists and contains a foreign
hostname, the program assumes that another instance is running on a different machine
and exits.

<span id="font"></span>

## Fonts

By default, desktop settings are used. However, a different font type and size
available on the system can be selected in the settings dialog.

<span id="shortcuts"></span>

## Shortcuts

The program generally supports various keyboard shortcuts for quick operation.
In the browser version, most shortcuts are disabled by default, since there can
be conflicts with the browser's own shortcuts (depending on the chosen browser,
and shortcuts typically work better when using the standalone progressive web app).
In all other cases, shortcuts are enabled by default and use Ctrl as the modifier key.
Shortcuts can be enabled or disabled in the settings dialog under the "Shortcuts" tab.
The modifier key used can also be adjusted there.

<span id="win"></span>

## Notes for Windows users

On Windows, all configuration files are looked for under `H:\.sctime`. The
generated time shell scripts and XML files are also stored there.

The password for the time database is expected in the file `H:\.Zeit` (if not
configured in `settings.xml`).

In general, every occurrence of `~` (tilde) as a placeholder for the home
directory on UNIX can be mentally replaced by `H:` in a Windows context.

<span id="ios"></span>

<!--
## Notes for Mac users

- `scTime.app` for the Mac includes Qt and the PostgreSQL client library as
  so-called private frameworks and is therefore self-contained, requiring no
  external components.
- `QODBC` is not available as a data source (at least not without external software components).

- The data source `QPSQL` is available. If the username on the Mac differs from the
  one in the time database, the module must be configured with the correct username
  via `settings.xml`. For further details, see [Data sources: databases](#datasource_database).

- If the account and on-call lists are provided as individual files, they must be in
  UTF-8 format. (More precisely: the program expects these files to use the encoding
  reported by `locale charmap`.) They are best generated with a call of the following
  form:
```
ssh user@linuxserver "LC_CTYPE=de_DE.UTF-8 zeitkonten --separator=\"|\" --sonderzeiten --psp --mikrokonten" >$HOME/.sctime/zeitkonten.txt
```

Password-free login is assumed here.
-->

<span id="datasources"></span>

## Data sources (power users)

The program loads the account list and the list of on-call categories
using one of the following methods:

- `QPSQL`: Qt database driver for Postgres
- `QODBC`: Qt database driver; uses ODBC data source `postgres_zeit`
- `command`: uses the commands `zeitkonten` and `zeitbereitls`. Has no effect on
  Windows, as the necessary commands are not available there.
- `json`: reads the file `sctime-offline.json` from the configuration directory
  (usually `~/.sctime`)
- `file`: Deprecated. Reads the files `zeitkonten.txt`, `sonderzeitls.txt` and
  `zeitbereitls.txt` from the configuration directory (usually `~/.sctime`)

Methods are tried in this order until the first one succeeds. Common reasons for
failure are missing files or settings. The errors that occur can be viewed under
*Help → Messages* in the menu and provide hints about the causes.

The attribute `names` of the XML element `sctime/general/backends` in `settings.xml`
can be used to reconfigure the default order of the modules. The method names listed
above are given in the desired order, separated by spaces. sctime will then try them
in the order they are listed.

Alternatively, the order can also be specified using the option `--datasource=NAME`.
If the option is given multiple times, modules are tried in the order they appear on
the command line.

<span id="datasource_command"></span>

### Data source `command`

The commands called are:

    zeitkonten --mikrokonten --psp --sonderzeiten --separator='|'
    zeitbereitls --separator='|'
    sonderzeitls --separator='|'

These cannot currently be configured.

### Data source `json`

All necessary data is contained in a single JSON file. The default path to the file
is `~/.sctime/sctime-offline.json`. This can be changed using the `--offlinefile`
option.

TODO Flo: syntax json

### Data source `file`

The files are expected at `~/.sctime/zeitkonten.txt`,
`~/.sctime/sonderzeiten.txt` and `~/.sctime/zeitbereitls.txt`.

Alternative paths can be specified using the parameters `--zeitkontenfile`,
`--sonderzeitenfile` and `--bereitschaftsfile` when launching the program.

To provide the files with the required content, the commands listed under
[Data source command](#datasource_command) can be called manually.

<span id="datasource_database"></span>

### Data sources `QPSQL` and `QODBC` (databases)

The following attributes of the element `sctime/general/backends/database` can be
used to configure the database data source parameters:

- `server`: the name of the server to contact
- `name`: the name of the database on the server
- `user`: the username to use. If the attribute is not specified or the value is
  empty, the operating system login name is used.
- `password`: the database user's password. If the attribute is absent or empty, the
  program searches for a file `~/.Zeit` (case-sensitive; on Windows `H:\.Zeit`) and
  uses its first line (without the line break) as the password. If the password file
  does not exist either, the username is used as the password.

<span id="offline"></span>

## Partial offline usage (with database)

The search order for account data described above also enables partial offline usage:
as long as the database can be reached, current account data is obtained from there
at each launch. Only if the database is unreachable will the program fall back to the
files in `~/.sctime`. These files can therefore be prepared for offline use while
still automatically using the database when available.

There are two things to keep in mind, however:

1. The files should be updated regularly or at least before each planned offline use.
2. The connection attempt to the database can cause long wait times at program startup
   and when reloading the account list. This is especially the case when you are in a
   network where the configured database server name can be resolved but the server
   itself is unreachable because a firewall silently drops traffic to it. In such a
   case, the search order can be temporarily changed or permanently reconfigured as
   described under [Data sources](#datasources).


<span id="portabel"></span>

## Portable edition (USB stick)

sctime can be launched from a USB stick without any problems. However, on Windows,
the drive letter assigned to a USB stick changes each time it is connected to a
different computer. As a result, the default `H:\.sctime` for locating settings
files will rarely be correct.

As a workaround, a useful property of Windows shortcuts can be exploited: when a
program launched via a shortcut has an empty working directory field, its current
directory will be the directory of the shortcut itself. A shortcut is therefore
created at the root of the USB stick containing the following call to sctime with
exclusively relative paths:

```
\sctime\sctime.exe --configdir=\.sctime
```

The working directory field must be left completely empty.

sctime launched this way will look for all its files in the `.sctime` subdirectory
of the USB stick, regardless of which drive letter it currently has.

UNIX/Linux users simply ensure by an appropriate means that the USB stick is always
mounted at the same path.
