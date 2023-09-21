# Embedded-Base
Embedded-Base is a collection of drivers & middleware designed for use across various systems.

This repository houses custom drivers and middleware that can be utilized in multiple projects.

To use this repository in any project, it should be set up as a submodule.

## Getting Started

#### 1. Clone Embbeded-Base:
   ~~~
   git clone https://github.com/Northeastern-Electric-Racing/Embedded-Base.git
   ~~~

#### 2. Initialize submodule
   Within the application directory, run the following command to initialize the submodule:
   ~~~
   git submodule update --init
   ~~~

## Development Guidelines

When developing in the parent directory, it's recommended to frequently run the following command to update all submodules in case changes have been made:
~~~
git submodule update --remote
~~~


**When making changes to a driver located within Embedded-Base, always make those changes directly within the Embedded-Base repository. Avoid making changes to a submodule from the parent directory.** Although it's technically possible, doing so can lead to disorganization, which we want to avoid.

This approach ensures that changes to Embedded-Base are tracked properly and can be easily integrated into your projects using it as a submodule.


