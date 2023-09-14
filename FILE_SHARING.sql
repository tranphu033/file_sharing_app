-- MySQL dump 10.13  Distrib 8.0.31, for Linux (x86_64)
--
-- Host: localhost    Database: FILE_SHARING
-- ------------------------------------------------------
-- Server version	8.0.31-0ubuntu0.22.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `FILE_SHARING`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `FILE_SHARING` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci */ /*!80016 DEFAULT ENCRYPTION='N' */;

USE `FILE_SHARING`;

--
-- Table structure for table `fgroup`
--

DROP TABLE IF EXISTS `fgroup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `fgroup` (
  `groupID` int NOT NULL AUTO_INCREMENT,
  `gname` varchar(50) DEFAULT NULL,
  `detail` varchar(300) DEFAULT NULL,
  `folderID` int DEFAULT NULL,
  `gcode` char(7) DEFAULT NULL,
  `ownerID` int DEFAULT NULL,
  PRIMARY KEY (`groupID`),
  UNIQUE KEY `folderID` (`folderID`),
  UNIQUE KEY `gcode` (`gcode`),
  KEY `ownerID` (`ownerID`),
  CONSTRAINT `fgroup_ibfk_1` FOREIGN KEY (`ownerID`) REFERENCES `user` (`userID`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `fgroup`
--

LOCK TABLES `fgroup` WRITE;
/*!40000 ALTER TABLE `fgroup` DISABLE KEYS */;
INSERT INTO `fgroup` VALUES (1,'group1','this is group 1',1,'phu3333',1),(2,'group2','this is group 2',2,'khiem33',2),(3,'group3','this is group 3',3,'phuc333',3);
/*!40000 ALTER TABLE `fgroup` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `file`
--

DROP TABLE IF EXISTS `file`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `file` (
  `fileID` int NOT NULL AUTO_INCREMENT,
  `fname` varchar(30) DEFAULT NULL,
  `folderID` int DEFAULT NULL,
  `fpath` varchar(200) DEFAULT NULL,
  `crtTime` datetime DEFAULT NULL,
  `ownerID` int DEFAULT NULL,
  PRIMARY KEY (`fileID`),
  KEY `folderID` (`folderID`),
  KEY `ownerID` (`ownerID`),
  CONSTRAINT `file_ibfk_1` FOREIGN KEY (`folderID`) REFERENCES `folder` (`folderID`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `file_ibfk_2` FOREIGN KEY (`ownerID`) REFERENCES `user` (`userID`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `file`
--

LOCK TABLES `file` WRITE;
/*!40000 ALTER TABLE `file` DISABLE KEYS */;
INSERT INTO `file` VALUES (1,'file1',1,'./file_groups/group1/file1','2022-12-14 17:48:43',1),(2,'fileabc',4,'./file_groups/group1/fol1/fileabc','2022-12-14 17:49:05',2),(3,'file1',2,'./file_groups/group2/file1','2022-12-14 17:49:13',2),(4,'flower.jpg',5,'./file_groups/group2/image/flower.jpg','2022-12-14 17:51:15',3),(5,'khiem_file',3,'./file_groups/group3/khiem_file','2022-12-14 17:49:23',3),(6,'phuc_file',3,'./file_groups/group3/phuc_file','2022-12-14 17:50:08',2),(7,'space.jpg',3,'./file_groups/group3/space.jpg','2022-12-14 17:51:15',1);
/*!40000 ALTER TABLE `file` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `folder`
--

DROP TABLE IF EXISTS `folder`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `folder` (
  `folderID` int NOT NULL AUTO_INCREMENT,
  `fol_name` varchar(30) DEFAULT NULL,
  `fol_path` varchar(200) DEFAULT NULL,
  `crtTime` datetime DEFAULT NULL,
  `ownerID` int DEFAULT NULL,
  PRIMARY KEY (`folderID`),
  KEY `ownerID` (`ownerID`),
  CONSTRAINT `folder_ibfk_2` FOREIGN KEY (`ownerID`) REFERENCES `user` (`userID`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `folder`
--

LOCK TABLES `folder` WRITE;
/*!40000 ALTER TABLE `folder` DISABLE KEYS */;
INSERT INTO `folder` VALUES (1,'group1','./file_groups/group1','2022-12-14 17:48:17',1),(2,'group2','./file_groups/group2','2022-12-14 17:48:20',2),(3,'group3','./file_groups/group3','2022-12-14 17:48:24',3),(4,'fol1','./file_groups/group1/fol1','2022-12-14 18:18:32',1),(5,'image','./file_groups/group2/image','2022-12-14 18:19:15',3);
/*!40000 ALTER TABLE `folder` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `group_member`
--

DROP TABLE IF EXISTS `group_member`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `group_member` (
  `groupID` int NOT NULL,
  `userID` int NOT NULL,
  `role` int DEFAULT NULL,
  PRIMARY KEY (`groupID`,`userID`),
  KEY `userID` (`userID`),
  CONSTRAINT `group_member_ibfk_1` FOREIGN KEY (`groupID`) REFERENCES `fgroup` (`groupID`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `group_member_ibfk_2` FOREIGN KEY (`userID`) REFERENCES `user` (`userID`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `group_member`
--

LOCK TABLES `group_member` WRITE;
/*!40000 ALTER TABLE `group_member` DISABLE KEYS */;
INSERT INTO `group_member` VALUES (1,1,1),(1,2,2),(1,3,2),(2,1,2),(2,2,1),(2,3,2),(3,1,2),(3,2,2),(3,3,1);
/*!40000 ALTER TABLE `group_member` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `parent_folder`
--

DROP TABLE IF EXISTS `parent_folder`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `parent_folder` (
  `folderID` int NOT NULL,
  `pfolderID` int NOT NULL,
  PRIMARY KEY (`folderID`,`pfolderID`),
  KEY `pfolderID` (`pfolderID`),
  CONSTRAINT `parent_folder_ibfk_1` FOREIGN KEY (`folderID`) REFERENCES `folder` (`folderID`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `parent_folder_ibfk_2` FOREIGN KEY (`pfolderID`) REFERENCES `folder` (`folderID`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `parent_folder`
--

LOCK TABLES `parent_folder` WRITE;
/*!40000 ALTER TABLE `parent_folder` DISABLE KEYS */;
INSERT INTO `parent_folder` VALUES (4,1),(5,2);
/*!40000 ALTER TABLE `parent_folder` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `user` (
  `userID` int NOT NULL AUTO_INCREMENT,
  `uname` varchar(60) DEFAULT NULL,
  `passwd` varchar(100) DEFAULT NULL,
  `name` varchar(60) DEFAULT NULL,
  `phone` char(10) DEFAULT NULL,
  `email` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`userID`),
  UNIQUE KEY `uname` (`uname`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user`
--

LOCK TABLES `user` WRITE;
/*!40000 ALTER TABLE `user` DISABLE KEYS */;
INSERT INTO `user` VALUES (1,'vanphu','phuite6','phu','0333333333','phuite6@gmail.com'),(2,'minhphuc','phucite6','phuc','0333333334','phucite6@gmail.com'),(3,'ngockhiem','khiemite6','khiem','0333333335','khiemite6@gmail.com');
/*!40000 ALTER TABLE `user` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2023-01-19 17:28:01
