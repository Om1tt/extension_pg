/* tmp--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION tmp" to load this file. \quit

-- Convert temperature
CREATE FUNCTION convert_temperature(double precision, text, text) 
RETURNS double precision
AS 'MODULE_PATHNAME', 'convert_temperature'
LANGUAGE C STRICT;