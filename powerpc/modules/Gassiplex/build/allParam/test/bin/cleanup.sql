DELETE FROM daq.store_int WHERE setup IN (
    'i_store',
    'i_store_existing',
    'i_store_multiple'
);
DELETE FROM daq.store_string WHERE setup IN (
    's_store',
    's_store_existing',
    's_store_multiple',
    's_store_whitespace'
);
QUIT
