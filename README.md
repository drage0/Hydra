# Hydra

Write out HTML from (modified) Markdown.

## Clarity

The software is very simple to understand. Go ahead and change it to suit your needs.

## Markdown language

By default, the Markdown language Hydra converts is modified. It has limitations.

### Images

All images *must* be on a separate line.

### Sprites (inline images)

A paragraph can being with an image from a sprite sheet.
Use `¬{sprite}` notation to specify what sprite to draw.
Sprites are defined in the CSS.

Example: `#wyvern{background-position: -128px 0}`.

### Special text (fancy text)

For the header text you may want to use a different font, colour or whatnot.

Wrapping a text in `£{hello text}` causes it to use a different style.
