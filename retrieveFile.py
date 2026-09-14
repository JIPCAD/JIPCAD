import requests
import sys

def retrieveFileOnline(filename):
    filename = filename.replace(".nom","").replace("\\","/")
    url = f'https://raw.githubusercontent.com/JIPCAD/JIPCAD/refs/heads/master/ExampleFiles/{filename}.nom'
    # Send GET request to the URL
    response = requests.get(url)

    if response.status_code != 200:
        print("Link failed, returning...")
        return ""

    # Parse the HTML content
    result = response.text.replace("\n", "").strip()
    print(result)

if __name__ == "__main__":
    if len(sys.argv) > 1:
        userInput = sys.argv[1] # Get user input from the command line argument from MainWindow.cpp
        retrieveFileOnline(userInput)