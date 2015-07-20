#!/bin/bash

#echo "1. Installing packages...It could take a while"
#sudo npm install sqlite3
#sudo npm install bookshelf
#sudo npm install express
#sudo npm install lodash
#sudo npm install body-parser
#sudo npm install bluebird
#sudo npm install knex
#sudo npm install cors
#echo "   Packages installed"

#sleep 1

echo "2. Creating database... "
sqlite3 database.sqlite < database.sql
echo "   database created"

sleep 1

echo "3. Starting server service..."
node server.js