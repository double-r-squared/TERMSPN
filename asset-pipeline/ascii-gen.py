from ascii_magic import AsciiArt
import os

ASCII_CHARS = " █▓▒░ "

# OPTION 3: Detailed gradient (more shades)
# ASCII_CHARS = " $@B%8&*/\\|()1{}[]?-_+~<>i!lI;:,\. "

# OPTION 4: Minimal (high contrast)
# ASCII_CHARS = " @$%=\+*/+-;. "

# OPTION 5: Numbers only
# ASCII_CHARS = "0123456789"

# Width of ASCII art
ASCII_WIDTH = 70


path = "/Users/NateAlmanza2022/TERMSPN/asset-pipeline/team_logos"
out = "/Users/NateAlmanza2022/TERMSPN/assets"
files = [f for f in os.listdir(path) if os.path.isfile(os.path.join(path, f))]
print(f"Files found: {files}")
print(f"\nUsing character set: {ASCII_CHARS}\n")
print("=" * 60)

for file in files:

    name = os.path.splitext(file)[0]  # Removes extension
    
    print(f"\nPROCESSING: {file}")
    print("-" * 40)
    
    # Create ASCII art with custom character set
    my_art = AsciiArt.from_image(os.path.join(path, file))
    
    # Use the 'char' parameter to set your custom characters
    my_art.to_file(
        path=os.path.join(out, name + ".txt"),
        columns=ASCII_WIDTH,
        monochrome=True,
        enhance_image=False,
        char=ASCII_CHARS  # <-- THIS CHANGES THE CHARACTERS!
    )
    
    print("\nDONE\n")

print("=" * 60)
print("All files processed!")