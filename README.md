# SkorCam

WebCam Partiture Reader and Player - Made with OpenCV

This is a project for the Human-Computer Interaction Course of the UCAB's Informatics Engineer career.

it consist of a software written in C++ that reads a special partiture through the WebCam of the computer, and plays the melody written in said partiture. It was developed with the help of the C++ library OpenCV, a library specialized in computer vision. For more information on OpenCV, please refer to the next link:

https://docs.opencv.org/3.0-beta/doc/tutorials/tutorials.html

The partiture needs to have certain characteristics for the software to work:
 
 * The lines of the staff needs to be orange. the more closer to #ff8c35, the better.
 * The musical shapes ( Whole note, half note, quarter note and quaver ) needs to be blue, arround #2818bc.
 
The program was tested in a big partiture ( 0.5m x 2m ), with the webcam at a distance of 2m. But in theory, it should work with partitures of any size, given that its close enough to the camera and the lines aren't super thin. 

the algorithm is something like this: the webcam reads frames continuously until it sees a red square (that's the 'play' sign the user gives to the program), and when it sees it, it begins to process the partiture in the given frame. the image (with the partiture in it) is passed through a filtering process, to clean it and identify its parts.

The program looks for the five orange lines and makes a reference system where the notes should be positioned. Meanwhile, it looks for blue figures and pass them through an SVM (Support Vector Machine), to identify the kind of the figure (using a learning algorithm) and the position relative to where the lines are. Once the notes (and its kind) are identified, and the position is known, the program plays the melody. 

Limitations of the software:

* It works better with stable, clear and white light.
* Heavy shadows are a problem.
* The lines should be as thicker as possible.
* only works with four kind of notes ( Whole note, half note, quarter note and quaver ). This is because of how the SVM was trained, but     adding another note shouldn't be hard: just take some pictures of the note and train the SVM again.
* only works in G Cleff.
* does not support Bars.

Included in the repository: Source code, trained SVM ( in the form of an XML ) and the data used to train the SVM. For the program to run, is necesary to install OpenCV in your machine.


 
