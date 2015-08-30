/*
  database creation script for the server in Raspberry Pi
*/

PRAGMA foreign_keys = OFF;

----------------------------------------------------------------------------------------------
-- Table structure for pet
----------------------------------------------------------------------------------------------
CREATE TABLE "pet" (
"id"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
"name"  TEXT(10)
);

-------------------------------
-- Records of pet
-------------------------------
INSERT INTO "pet" VALUES (1, 'Cooper');

----------------------------------------------------------------------------------------------
-- Table structure for event_type
----------------------------------------------------------------------------------------------
CREATE TABLE "event_type" (
"id"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
"name"  TEXT(10)
);

-------------------------------
-- Records of pet
-------------------------------
INSERT INTO "event_type" VALUES (1, 'bark');
INSERT INTO "event_type" VALUES (2, 'movement');

----------------------------------------------------------------------------------------------
-- Table structure for event
----------------------------------------------------------------------------------------------
CREATE TABLE "event" (
"id"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
"pet_id" INTEGER NOT NULL,
"event_id" INTEGER NOT NULL,
"date"  TEXT(10),
"time"  TEXT(10),
FOREIGN KEY ("pet_id") REFERENCES "pet" ("id"),
FOREIGN KEY ("event_id") REFERENCES "event_type" ("id")
);

-------------------------------
-- Records of event
-------------------------------
INSERT INTO "event" VALUES (5, 1, 1, '2010-10-16', '12:30:50');
INSERT INTO "event" VALUES (4, 1, 1, '2010-10-16', '12:30:45');
INSERT INTO "event" VALUES (1, 1, 2, '2010-10-15', '10:30:45');
INSERT INTO "event" VALUES (6, 1, 2, '2012-10-15', '20:30:45');
INSERT INTO "event" VALUES (3, 1, 2, '2010-10-15', '19:30:45');
INSERT INTO "event" VALUES (2, 1, 1, '2010-10-15', '13:30:45');
INSERT INTO "event" VALUES (9, 1, 2, '2015-10-05', '11:00:45');
INSERT INTO "event" VALUES (12, 1, 1, '2015-10-06', '18:00:45');
INSERT INTO "event" VALUES (7, 1, 1, '2014-10-05', '00:00:48');
INSERT INTO "event" VALUES (10, 1, 1, '2015-10-05', '11:10:45');
INSERT INTO "event" VALUES (11, 1, 2, '2015-10-06', '15:00:45');
INSERT INTO "event" VALUES (8, 1, 1, '2014-10-05', '01:00:45');