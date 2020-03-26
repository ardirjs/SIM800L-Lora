SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";

CREATE TABLE `tb_database` (
  `id` int(10) NOT NULL,
  `switch` varchar(10) NOT NULL,
  `sensor` varchar(20) NOT NULL,
  `times` varchar(20) NOT NULL,
  `dates` varchar(20) NOT NULL,
  `maplink` varchar(50) NOT NULL,
  `region` varchar(50) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

INSERT INTO `tb_database` (
  `id`,
  `switch`,
  `sensor`,
  `times`,
  `dates`,
  `maplink`,
  `region`
) VALUES (
  1,
  '1',
  '[0.06,0.14,0.04]',
  '01:23:29',
  '23/03/2020',
  'https://goo.gl/maps/mwKJdCyWPQDFHLix8',
  'Jln. Banyuwangi Kec. Banyuwangi'
);

ALTER TABLE `tb_database`
  ADD PRIMARY KEY (`id`);

ALTER TABLE `tb_database`
  MODIFY `id` int(10) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
COMMIT;
