# sctime

1. [Introduction](#intro)
1. [Settings](#usage)
1. [Account tree](#tree)
1. [Active account](#active)
1. [Editing times in the GUI](#edit)
1. [Saving times](#save)
1. [Default comments, micro accounts, predefined comments](#comments)
1. [Attendance time recording](#fromto)
1. [On-call times](#bereit)
1. [Special remuneration times](#sonder)
1. [Type and PSP](#psp)
1. [Tips & Tricks](#tipps)


<span id="intro"></span>

## Introduction

sctime records working times in real time by continuously accumulating elapsed time
on a time (sub)account actively selected by the user.
- Available time accounts are centrally managed, change as needed, and are therefore re-read on each program launch.
- sctime continuously writes the times to a time file `zeit-YYYY-MM-DD.sh`.

Times can be annotated with comments while being recorded in the GUI to describe
what was done during that time.
- For each subaccount, multiple times (in the GUI: rows) can be recorded with different comments each; the time file will then contain correspondingly many distinct entries.
- Each of these subaccount comment rows has its own subaccount editing window.
- As described under [Default comments](#comments), standard comments can be centrally defined for all subaccounts, which the user only needs to activate from the comment drop-down list (double-click).

The user retains full control over the recorded times and can edit their time files `zeit-YYYY-MM-DD.sh` afterwards.

<span id="usage"></span>

## Settings

| Option | Default | Description |
| --- | --- | --- |
| Re-use previous day's comments | - | saves all comments for the next day. |
| Poweruser view | - | provides additional convenience options for billable times in the GUI. |
| Activate account by single click | - | Default: double-click required |
| Show account type | - | Displays the account type |
| Show PSP element number | - | Displays the PSP element |
| Automatically use default comment if it is unique | - | if the sctime GUI finds only one comment, it uses it automatically |
| Activate drag 'n' drop | X | enables drag 'n' drop for times |
| Show sum in personal accounts | - | shows totals for accounts and for subaccounts with multiple comments |
| Show special remuneration selector | - | Shows the special remuneration selection in the account dialog |
| Warn if comment doesn't conform to ISO8859-1 | X | on input of unusual characters, to prevent later problems during evaluation |
| Sort by comment text (instead by number) | - | sorts multiple comment rows of subaccounts by first character |
| Stay offline after initialization | - | **Web GUI only:** always saves time files in the browser |
| Do not write consolidated intervals in sh file | - | does not write start and end times to the time files |

<span id="tree"></span>

## Account tree

The account hierarchy is a tree structure: Department → Account → Subaccount

Under *Accounts* there are two entry points:
- *All accounts*
- *Personal accounts*

*All accounts* shows all centrally provided accounts.
- Times can only be recorded at the lowest level (subaccount).
- When the structure changes, run *Account → Reload account list (Ctrl+R)*.
- A double-click on a subaccount selects it as the active one (see [Settings](#usage)). sctime immediately starts recording time for that subaccount.

Adding subaccounts to *Personal accounts* and the subaccount editing window:
- A right-click (or double-click when single-click activation is enabled) on a subaccount row opens the subaccount editing window.
- Check *Add to personal accounts*.
- Enter or select a comment. If *Re-use previous day's comments* is enabled under *Settings → Settings → General*, the sctime GUI saves all comments so they do not need to be re-entered.
- Edit (billable) times.
- If a description and a responsible person are centrally defined for the subaccount, this information is shown here.

Recording additional times on the same subaccount with a different comment:
- *Account → Add entry* creates an additional row for the active subaccount in which a different comment can be entered or selected.
- From two rows onwards, the subaccount can be collapsed and expanded in the GUI.

<span id="active"></span>

## Active account

As soon as the program is started:
- time recording begins
- on the subaccount marked with a check mark as active.

Depending on the setting, a single or double-click (default) activates the desired subaccount.

<span id="edit"></span>

## Editing times in the GUI

The current day or a past day (*Time → Select date...*) can be modified:
- directly in the GUI (icons for increasing/decreasing time)
- in the editing window of the respective subaccount

The *Total time* at the bottom right shows how many minutes the times have been changed in total (+ or −).
- This facilitates redistributing effort between different subaccounts.
- If no difference is shown, the redistribution was successful.
- If too many or too few times have been recorded (forgotten breaks), *Time → Reset Difference Ctrl+N* can be used as needed to reset the current increase/reduction of total hours back to 0.

<span id="save"></span>

## Saving times

During operation, the sctime GUI saves automatically every five minutes.

The current times and settings are saved immediately:
- on program exit
- when the user explicitly saves (via button, Ctrl+S, *Account → Save*)


<span id="comments"></span>

## Default comments, micro accounts, predefined comments

There are good reasons to provide default comments:
- Convenience: the user only needs to select from a list.
- Consistency: makes evaluation easier.

The three terms above all mean the same thing:
- Comments can be centrally predefined for all subaccounts for selection in the subaccount editing window of the GUI.
- Default comments can be provided by the administrator, or all users may do so using an appropriate command-line tool.
- Users must reload the account list to receive new default comments.

Micro accounts add another level to the account tree structure:
- For example, a default comment `Bugfix:` can be defined.
- The user can enter further details after the colon.
- Using `Bugfix:` allows all time for that topic to be evaluated together.


<span id="fromto"></span>

## Attendance time recording

The sctime GUI records attendance times to document compliance with
occupational health and safety regulations, and to generate warning dialogs
reminding users of missing breaks if necessary.

A cleaned-up summary of attendance times is stored in `zeit-YYYY-MM-DD.sh` files for documentation or saved in the browser (web GUI).
- The sctime GUI automatically counts all periods as attendance time during which it is running and not paused.
- The recorded times can be viewed and corrected if necessary via *Time → Attendance times*.

If the total time booked to accounts has been changed (e.g. a few minutes added to an account because a break was not stopped in time), calling *Time → Reset Difference* will offer to also adjust the start of the current work interval in the attendance times accordingly.


<span id="bereit"></span>

## On-call times (stamp icon)

On-call categories are centrally defined by the administrator.

To record on-call times:
- Select the time entry to which the on-call period belongs.
- Choose *Remuneration → Set on-call times... Ctrl+B*.
- Select one or more categories from the list of available on-call categories.
- Then click *OK*.

To remove an on-call selection, click the stamp again, deselect the chosen categories, and confirm with *OK*.

<span id="sonder"></span>

## Special remuneration times (moon icon)

Special remuneration times are worked hours performed at "unusual" times such as at night or on public holidays. Supplements may be invoiced for special remuneration times. They should therefore only be entered in accordance with the currently applicable rules and agreements. Special remuneration categories are centrally defined by the administrator.

To record special remuneration times:
- Select the time entry to which the special remuneration applies.
- Choose *Remuneration → Set special remuneration categories... Ctrl+T*.
- Select one or more categories from the list of available special remuneration categories.
- Then click *OK*.

To remove a selection, click the moon again, deselect the chosen categories, and confirm with *OK*.

If accounts are centrally marked with type 'x' or 'o', special remuneration times cannot be set on them.

### Special remuneration mode

A special remuneration mode links a special remuneration category to the automatic assignment of that category for all subsequently recorded times until the mode is deactivated — for example, night work:
- A special remuneration mode can only be defined centrally.
- When special remuneration modes are configured, users will find them in *Remuneration* below *Set special remuneration categories...*.
- If an existing time entry is activated where this category is not already set, a new entry will be created with the category set accordingly, so that the special remuneration time remains separate from normal time recording.

**Important:** special remuneration modes are a convenience feature only. They do not check whether the employee is entitled to claim special remuneration in the current situation. Employees must still be aware of and comply with the currently applicable regulations at their company.

<span id="psp"></span>

## Type and PSP

Both can be centrally defined for each subaccount to allow more detailed evaluation/grouping of accounts.
- The user cannot change these values.
- They are hidden in the GUI by default — see [Settings](#usage).

<span id="tipps"></span>

## Tips & Tricks

### Drag 'n' Drop

Times can be moved between subaccounts and entries using drag 'n' drop.

Holding down the *Shift* key also moves the set comments.

### Logging

Under *Help → Messages* in the menu, log messages about data sources and similar events can be viewed.

### Color-coding accounts

*Account → Choose/remove background color* allows (sub)accounts to be highlighted with a background color.
