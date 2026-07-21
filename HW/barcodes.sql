create table barcodes (
	id bigint primary key,
	code varchar(159),
	val varchar(255)
);

insert into barcodes(id, code, val)
values (11, 'dfc', 'vgn'), (123, 'xsd', 'fgh'), (56, 'zas', 'rty'), (1235, 'rew', 'ytr');
