Install

1. copy the newsflash folder to sites/all/themes/
2. go to apperance an click enable and set as default
3. click on settings an make your settings
thats all

some special fetures

pickstyle
we have implementet a pickstyle.js
how to use 
make a block wich will shown only on front(very recomment)
then fill in 

<a href="/" onclick="pickstyle('blue')">Blue</a>
<a href="/" onclick="pickstyle('copper')">Copper</a>
<a href="/" onclick="pickstyle('green')">Green</a>
<a href="/" onclick="pickstyle('black')">Black</a>
<a href="/" onclick="pickstyle('red')">Red</a>
<a href="/" onclick="pickstyle('aqua')">Aqua</a>
<a href="/" onclick="pickstyle('violet')">Violet</a>

now can each user set her/his own color style 
this will sotored in a cookie

Local Content File Name
the local content file means a CSS file
enter the locaton where is stored the css file
eg.:
sites/all/themes/newsflash/css/icons.css

you can use Typography
use follow examples
<span class="alert">This is the alert style. To use this style in you node or block content, wrap your text with the following HTML: <strong>&lt;span class="alert"&gt;...&lt;/span&gt;</strong></span><br />
<span class="help">This is the help style. To use this style in you node or block content, wrap your text with the following HTML: <strong>&lt;span class="help"&gt;...&lt;/span&gt;</strong></span><br />
<span class="note">This is the note style. To use this style in you node or block content, wrap your text with the following HTML: <strong>&lt;span class="note"&gt;...&lt;/span&gt;</strong></span><br />
<span class="xfer">This is the xfer style. To use this style in you node or block content, wrap your text with the following HTML: <strong>&lt;span class="xfer"&gt;...&lt;/span&gt;</strong></span>