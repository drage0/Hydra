TITLE: Hydra
JAVASCRIPT: res/main.js
KEYWORDS: Hydra,Software
DESCRIPTION: Hydra software example.
SPRITES: res/sheet.gif
~Hydra software example.

# Hydra

## -h is for help

This document is an example usage of the software. All functioanlity is used.
The document can have embeddded HTML syntax, use this to bold, italicise or manipulate the text in any other way the software doesn't allow.
Keep in mind that letters that have special meaning in HTML syntax, such as “&lt;” and “&gt;”, have to be written in a special way. Check the whole table |https://www.rapidtables.com/web/html/html-codes.html|here|.

${basilisk}<i>“Mhm.”</i>

## Images

Here a highly compressed image is shown. But fear not for the higher resolution image will start to load and show itself once the mouse hovers over the compressed one!

The images also have alt-text included.

![sand screenshot](res/sand_0.c.jpg)

## Listing

%
Number one
Number two
Number 3
Number |https://en.wikipedia.org/wiki/Flour|flour|
%

## Links & Sprites

${github} |https://github.com/drage0/Hydra|Hydra on GitHub|

${mellon} Erroneous sprite names always yield the first sprite.

## Embedded HTML
If simple text manipulation with tags such as “&lt;i&gt;” and “&lt;b&gt;” are not enough, lines between two “&amp;” will be copied over to the final HTML document.
&
<!-- This is embedded HTML sequence. All its contents will be copied to the output document. -->
<canvas id="c">(HTML5 canvas)</canvas>
<script>
var c = document.getElementById("c");
var ctx = c.getContext("2d");
var grd = ctx.createLinearGradient(0,0,200,0);
grd.addColorStop(0,"red");
grd.addColorStop(1,"white");
ctx.fillStyle = grd;
ctx.fillRect(10,10,150,80);
</script>
&
