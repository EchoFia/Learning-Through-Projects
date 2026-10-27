"""
 - Takes in an 'input.md' file that contains the contents of a Notion page
 - Converts the input into a HTML format where required:
    - Toggle lists
    - Bold Text
 - Exports to an 'output.txt' file
"""

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Import Modules ###

import re

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Misc Functions ###

def surround_bold(m):
    """ Used for replacing **...** to <strong>...</strong> """
    return f'<strong>{m.group()}</strong>'.replace('**', '')

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Converting to HTML ###

def toggle_to_toggle_html(md):
    """ Converts Notion's toggle to a HTML togle """

    i = 0
    while i < len(md) - 1:

        if md[i][:2] == "- " and md[i + 1].isspace():
            md[i] = '<details>\n<summary>' + md[i][2:] + '</summary>\n<ul>'

            # Search through all the next ones
            j = i + 1
            while j < len(md):

                if md[j].isspace():
                    md[j] = ''
                elif md[j][:4] != '    ':
                    j -= 1
                    break
                else:

                    # Replace all spaces at the start with '- '
                    # USE RE in the future

                    k = 1
                    while k < len(md[j]) and md[j][:k].isspace():
                        k += 1
                    md[j] = '<li>' + md[j][k - 1:] + '</li>'

                

                j += 1

            if j == len(md):
                md.append('</ul>\n</details>\n')
            else:
                md[j] += '</ul>\n</details>\n'
            i = j

        i += 1

    return md

def ul_to_toggle_html(md):
    """ Will convert bullet points with sub-points to a toggle list """

    i = 0
    while i < len(md) - 1:

        if md[i][:2] == "- " and md[i + 1][:6] == '    - ':
            md[i] = '<details>\n<summary>' + md[i][2:] + '</summary>\n<ul>'

            # Search through all the next ones
            j = i + 1
            while j < len(md):

                if md[j][:6] == '    - ':
                    md[j] = '<li>' + md[j][6:] + '</li>'
                # elif md[j][0:2] == '- ':
                else:
                    j -= 1
                    break                

                j += 1

            if j == len(md):
                md.append('</ul>\n</details>\n')
            else:
                md[j] += '</ul>\n</details>\n'

            i = j

        i += 1

    return md





def bold_to_html(md):
    pattern = r'\*\*.+\*\*'
    for i in range(len(md)):
        md[i] = re.sub(pattern, surround_bold, md[i])

    return md


### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Main ###

def main():

    # Extract raw information (copied from a Notion file)
    with open("input.txt", "r") as f:
        toggle_list = f.readlines()

    for i in range(len(toggle_list)):
        toggle_list[i] = toggle_list[i].replace('\n', '')

    # Convert **...** to <strong>...</strong>
    toggle_list = bold_to_html(toggle_list)

    toggle_list = toggle_to_toggle_html(toggle_list)
    toggle_list = ul_to_toggle_html(toggle_list)

    # Add back the removed '\n'
    ### NOT SURE ABOUT IGNORING BLANK STRINGS
    for i in range(len(toggle_list)):
        if toggle_list[i] != '':
            toggle_list[i] += '\n'

    with open("output.txt", "w") as f:
        f.writelines(toggle_list)

if __name__ == "__main__":
    main()