#!/bin/bash
protoc --cpp_out=. person.proto
protoc --cpp_out=. addressbook.proto
