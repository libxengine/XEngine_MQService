/*
 Navicat Premium Data Transfer

 Source Server         : QCloud
 Source Server Type    : MySQL
 Source Server Version : 80032 (8.0.32-0ubuntu0.22.04.2)
 Source Host           : 42.194.178.57:3306
 Source Schema         : XEngine_MQUser

 Target Server Type    : MySQL
 Target Server Version : 80032 (8.0.32-0ubuntu0.22.04.2)
 File Encoding         : 65001

 Date: 24/04/2023 15:00:41
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for KeyOwner
-- ----------------------------
DROP TABLE IF EXISTS `KeyOwner`;
CREATE TABLE `KeyOwner`  (
  `tszUserName` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '所属用户',
  `tszKeyName` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '拥有的队列名称',
  `tszCreateTime` datetime NOT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '创建时间',
  PRIMARY KEY (`tszKeyName`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for UserInfo
-- ----------------------------
DROP TABLE IF EXISTS `UserInfo`;
CREATE TABLE `UserInfo`  (
  `ID` int NOT NULL AUTO_INCREMENT,
  `tszUserName` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '用户名',
  `tszUserPass` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '密码',
  `tszEMailAddr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '电子邮件',
  `nPhoneNumber` bigint NULL DEFAULT NULL COMMENT '电话号码',
  `nIDNumber` bigint NULL DEFAULT NULL COMMENT '身份证',
  `nUserState` tinyint NOT NULL COMMENT '用户状态1在线,0离线',
  `nUserLevel` int NOT NULL COMMENT '用户等级,数字越小越高',
  `tszLoginTime` datetime NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '登录时间',
  `tszCreateTime` datetime NOT NULL COMMENT '注册时间',
  PRIMARY KEY (`ID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for UserKey
-- ----------------------------
DROP TABLE IF EXISTS `UserKey`;
CREATE TABLE `UserKey`  (
  `ID` int NOT NULL AUTO_INCREMENT,
  `tszKeyUser` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '用户名',
  `tszKeyName` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '用户绑定的KEY名称',
  `nKeySerial` bigint NOT NULL COMMENT '用户绑定KEY的序列号',
  `tszUPTime` datetime NOT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '最后更新时间',
  `tszCreateTime` datetime NOT NULL COMMENT '创建的时间',
  PRIMARY KEY (`ID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for UserTime
-- ----------------------------
DROP TABLE IF EXISTS `UserTime`;
CREATE TABLE `UserTime`  (
  `tszQueueName` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '队列名称',
  `nIDMsg` bigint NOT NULL COMMENT '队列唯一ID',
  `nIDTime` bigint NOT NULL COMMENT '发布时间',
  `bActive` tinyint NULL DEFAULT NULL COMMENT '是否激活过',
  `bBreak` tinyint NULL DEFAULT NULL COMMENT '是否跳过自身',
  `tszCreateTime` datetime NOT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '创建时间'
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = DYNAMIC;

SET FOREIGN_KEY_CHECKS = 1;
