The windows version is dependent on:
- SDL2-2.26.3
- SDL2-image-2.6.3
- SDL2_ttf-2.20.2

The Linux version uses older SDL2 libraries, so it uses:
- TTF_RenderText_Blended_Wrappped instead of TTF_RenderText_Solid_Wrapped
- SDLGetTicks instead of SDLGetTicks64

The "resources" folder should be in the program's working directory

If high_score.txt doesn't have a valid number in it the program will probably crash.

This wouldn't be possible without the tutorials at ==> https://lazyfoo.net/
Thank you Mr. Foo.
