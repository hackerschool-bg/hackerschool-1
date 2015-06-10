
CREATE TABLE ek_atte (
    ekatte_code character varying(5) NOT NULL,
    name character varying(25),
    kind smallint,
    oblast character varying(3),
    obstina character varying(5),
    kmetstvo character varying(8)
);

CREATE TABLE ek_kmet (
    kmetstvo_code character varying(8) NOT NULL,
    ekatte_center character varying(5),
    name character varying(25)
);

CREATE TABLE ek_obl (
    oblast_code character varying(3) NOT NULL,
    ekatte_center character varying(5),
    name character varying(25),
    region2_code character varying(4)
);

CREATE TABLE ek_obst (
    obstina_code character varying(5) NOT NULL,
    ekatte_center character varying(5),
    name character varying(25)
);

CREATE TABLE ek_reg2 (
    region_code character varying(4) NOT NULL,
    name character varying(25)
);

ALTER TABLE ONLY ek_atte
    ADD CONSTRAINT ek_atte_pkey PRIMARY KEY (ekatte_code);

ALTER TABLE ONLY ek_kmet
    ADD CONSTRAINT ek_kmet_pkey PRIMARY KEY (kmetstvo_code);

ALTER TABLE ONLY ek_obl
    ADD CONSTRAINT ek_obl_pkey PRIMARY KEY (oblast_code);

ALTER TABLE ONLY ek_obst
    ADD CONSTRAINT ek_obst_pkey PRIMARY KEY (obstina_code);

ALTER TABLE ONLY ek_reg2
    ADD CONSTRAINT ek_reg2_pkey PRIMARY KEY (region_code);

ALTER TABLE ONLY ek_atte
    ADD CONSTRAINT fk_atte_oblast FOREIGN KEY (oblast) REFERENCES ek_obl(oblast_code);

ALTER TABLE ONLY ek_atte
    ADD CONSTRAINT fk_atte_obstina FOREIGN KEY (obstina) REFERENCES ek_obst(obstina_code);

ALTER TABLE ONLY ek_kmet
    ADD CONSTRAINT fk_kmet_center FOREIGN KEY (ekatte_center) REFERENCES ek_atte(ekatte_code);

ALTER TABLE ONLY ek_obl
    ADD CONSTRAINT fk_obl_center FOREIGN KEY (ekatte_center) REFERENCES ek_atte(ekatte_code);

ALTER TABLE ONLY ek_obl
    ADD CONSTRAINT fk_obl_region FOREIGN KEY (region2_code) REFERENCES ek_reg2(region_code);

ALTER TABLE ONLY ek_obst
    ADD CONSTRAINT fk_obst_center FOREIGN KEY (ekatte_center) REFERENCES ek_atte(ekatte_code);
