QForms Overview ❮‹
==================

_30 Sep. 2007._

QForms is a small graphical toolkit to build html-like forms or
small user interfaces. QForms can be used in two ways: either
directly from the command line (to build shell script interfaces
for instance, like xmessage) or from a program. Those two cases
are described below.

**Disclaimer:**
QForms are still under development and a lot of features are missing,
nevertheless, the current version is stable and operational and can
be used for simple applications.

Forms are created as transient windows meaning they are not handled
by the window manager, if you somehow loose the focus on a form you
can only gain it again using the pointer (at last if you're using
`qwm`). To avoid this situation, a form will simply grab the pointer
until it is closed (submitted or discarded) and more generally it
will close itself if it looses the focus.

A left-click on the form submits it, a right-click discards it.
Pressing enter (and escape respectively) entails the same actions.
Use tab to switch between input controls. The text input control is
quite limited: you can delete characters from the end (backspace)
but you cannot use arrows. Press delete to erase the whole text.

Command Line Usage
------------------

You should have a binary named `qform` in the Quark archive;
otherwise please refer to the installation procedure to get it.
This is a command line tool allowing to dynamically create forms.

- By default the window is centered and its size is calculated from its content.
	Its position can be modified:
	you can either indicate absolute coordinates
	or stick the window on a screen side.
	`-x value|'l' for left|'c' for center (defaut)|'r' for right`
	`-y value|'t' for top|'c' for center (default)|'b' for bottom`
	See below for an example.

- Controls.
	Values for (alignment) are: `'l' (left), 'c' (center) or 'r' (right)`.
	Values for (font) are: `'s' (small), 'm' (medium) or 'b' (big)`.

	Available controls:
	- **label**, syntax: `-label (font) (alignment) (string)`.
		e.g. `$ qform -label big center 'Hello World'` or `$ qform -l b c 'Hello World'`
	- **horizontal separator**, syntax: `--`.
		e.g. `$ qform --`
	- **vertical space**, syntax: `-break`.
		e.g. `$ qform -break` or `$ qform -b`
	- **text input**, syntax: `-cmd (font) (alignment) (maxchar) (string)`.
		e.g. `$ qform -cmd medium center 10 'Default Value'` or `$ qform -c m c 10 'Default Value'`

	Obviously, controls can be stacked:
		e.g. `$ qform -l b c Test -- -cmd m c 10 test -x r -y b`

	Once the form is submitted, a list of each input value is displayed:

		$ qform -l b c Test -- -cmd m c 10 foo -cmd m c 10 bar
		foo
		bar
		$ 

	If the form is not submitted, there is no output.


Within a C Program
------------------

- First include `qform.c`.
	This is not a header, there is no multiple inclusion protection so be sure to include it once only.
- Build the form.
	For example to get the equivalent of `qform -l b c Test -- -cmd m c 10 test`:

		Form form=setupForm(0); // Argument 0 is the screen number.
		Control control;
		control.label=getLabel(2,1,"Test");
		pushControl(&form,control);
		control.bar=getBar(0);
		pushControl(&form,control);
		control.cmd=getCmd(1,1,10,"test");
		pushControl(&form,control);

- if you're not using X at all, simply call `showForm(&form)` as
   used in `qhelp.c`, else call `setup()` (perform the connection to X and
   load fonts), do your specific X part and call `showForm(&form)`
   when you need it. See `qrun.c` for an illustration.
   The main subtlety in this last case, it the presence of two
   event loops, which is not always easy to manage.
   `showForm()` returns 1 if the form is submitted, 0 otherwise.
   You can recall `showForm()` later without recreating the form object.
- Input control value can be retrieved as follows:

		char* s=form.controls[index].b.s;

	Where index is the value returned by `getCmd()`.

### API Reference

- `setupForm(screenid)`: create a new form.
- `getLabel(font,alignment,text)`:
	where font is 0 for small, 1 for medium, 2 for big,
	and alignment is 0 for left, 1 for center, 2 for right.
- `getBar(hidden)`:
	hidden is 1 for a standard break, 0 for a horizontal line.
- `getCmd(font,alignment,maxchar,defaultValue)`:
	Same values than label. getCmd() returns an integer which is
	the input control index, you need it to retrieve the control
	value when processing the form.
