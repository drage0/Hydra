/*
 * Define the data used to make the HTML files.
 * PAGE_WIDTH defines the maximum width of the line.
 * TEXT_COLOUR defines the colour of the text.
 */
#define LANGUAGE "en"
#define PAGE_WIDTH "42em"
#define TEXT_COLOUR "#fff2da"
#define LINK_COLOUR "#fa4040"
#define LINK_VISITCOLOUR "#aa2040"
#define BACKGROUND_COLOUR "#353C51"
#define AUTHOR "Aleksandar"
/* Charcater '%' has to be written as '%%' for it to not be recognised as a conversion charcater. */
#define STYLE "html{background:url(\"res/bg.jpg\");background-color:"BACKGROUND_COLOUR";background-repeat:repeat;font-family:\"Linux Libertine O\", \"Liberation Serif\", \"Times New Roman\", Times, serif;}\n\
body{margin:2em auto;max-width:"PAGE_WIDTH";line-height:1.6;font-size:1.2em;color:"TEXT_COLOUR";padding:0 10px;image-rendering:pixelated;image-rendering:crisp-edges}\
h1, h2{line-height:1.2; text-shadow: 2px 3px 0 #111}\n\
h1{font-size:2.5em}\n\
h2{font-size:2em;padding-top:1em}\n\
.f{padding-bottom:1em;font-size:2.8em;text-align:center;font-family:\"Palatino Linotype\",\"Book Antiqua\",Palatino,serif;font-style:italic;text-shadow: 2px 3px 0 #111}\
h3{padding-right: 1em; text-shadow: 1px 2px 0 #111}\n\
hr{margin-top: 2em; margin-bottom: 2em;border-color:#cfc2aa;color:#cfc2aa}\n\
img{width:100%%;height:auto}\n\
a{color:"LINK_COLOUR"}\n\
a:visited{color:"LINK_VISITCOLOUR"}\n\
.i{width:64px;height:64px;display:inline-block;vertical-align:middle;background:url(\"%s\")}\
#basilisk{background-position: 0 0}\
#github{background-position: -64px 0}"
