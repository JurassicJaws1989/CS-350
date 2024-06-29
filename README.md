# SNHU CS-350 Portfolio
## Emerging System Arch. & Tech. Final Project
### Justin M. Smith

# Summary of Project and Problem Description
In this project, we were asked to create a prototype of a working thermostat with the following attributes,
* An initialized timer to drive specific actions
* An interrupt to detect button presses in the prototype board
* An I2C peripheral that can handle reading the temperature sensor
* A GPIO peripheral that can handle the output via LED's
* A UART2 peripheral that will be used to simulate the data output to the eventual server
* A task scheduler that handles the functionality implementation

The prototype board being used for this project is the Texas Instruments TI CC3220x LAUNCHXL

All code was composed within TI Code Composer Studio

- We were required to create functioning code to handle all of the above functionality and end up with a working prototype
- We were also required to provide a task scheduler diagram and a written report

# What did you do particularly well?
One thing that exceeded my expectation was my ability to acheive the required functionality in the prototype at a code level. I do not have any experience with embedded systems, and have not
had a ton of experience writing in C. Due to this, I had some trepedations when reading through some of the requirements. I am proud of my ability to work through these challenges, and am also thankful for 
the provided resources, my intructor, and my class peers for all of the help while working through this project. 
# Where could you improve?
With my experience previously mentioned, I think one area of improvement is gaining a stronger fundamental understanding on the concepts. Especially when the peripherals are concerned. I think that my code could
have been cleaner, and better organized. While it did end up functional in the end, I am thinking that it could have been better commented and more readable.
# What tools and/or resources are you adding to your support network?
Throughout this project, and this class as a whole, I have been introduced to a multitude of new concepts and resources. While this has simply scratched the surface, I now have the tools equipped that enable me 
learn more. 
* More experience writing in C, which is useful for not only embedded systems but many other avenues
* Experience with microcontrollers and how to interact with them
* Experience with peripherals like UART, I2C, and GPIO
# What skills from this project will be particularly transferable to other projects and/or course work?
Many skills utilized within this project will easily transfer to other future projects, whether in education or in my professional career. Simply, the experience working with hardware and software together is incredibly 
beneficial for getting a more beneficial and well-rounded understanding on how components interact with one-another. Also, the extra experience with another programming language will only aim to increase my ability going forward.
# How did you make this project maintainable, readable, and adaptable?
As I mentioned above, this is one area where I feel I could have improved a bit. With that said, in terms of maintainability and readability, I tried to structure all of my code in a way where it was easy to read and follow. I tried to 
add as many comments as I could to ensure that I would understand what I was doing as I logged back in to continue working on the project. The code is as adaptable as I could make it, with all declarations taking place in well defined modular functions.



