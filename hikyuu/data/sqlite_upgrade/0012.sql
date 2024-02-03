BEGIN
TRANSACTION;

INSERT INTO `Market` (`marketid`,
                      `market`,
                      `name`,
                      `description`,
                      `code`,
                      `lastDate`,
                      `openTime1`,
                      `closeTime1`,
                      `openTime2`,
                      `closeTime2`)
VALUES (4,
        "OKX",
        "OKX数字货币交易所",
        "数字货币市场: Okx",
        "",
        0,
        0,
        0,
        0,
        0);

INSERT INTO `Stock` (`marketid`,
                     `code`,
                     `name`,
                     `type`,
                     `valid`,
                     `startDate`,
                     `endDate`)
VALUES (3, 'ADA-USDT',   'BTC-USDT现货',   10, 1, 20200102, 99999999),
       (3, 'AVAX-USDT',  'AVAX-USDT现货',  10, 1, 20200924, 99999999),
       (3, 'BCH-USDT',   'BCH-USDT现货',   10, 1, 20200102, 99999999),
       (3, 'BNB-USDT',   'BNB-USDT现货',   10, 1, 20221221, 99999999),
       (3, 'BTC-USDT',   'BTC-USDT现货',   10, 1, 20180112, 99999999),
       (3, 'DAI-USDT',   'DAI-USDT现货',   10, 1, 20200102, 99999999),
       (3, 'DOGE-USDT',  'DOGE-USDT现货',  10, 1, 20200102, 99999999),
       (3, 'DOT-USDT',   'DOT-USDT现货',   10, 1, 20200824, 99999999),
       (3, 'ETH-USDT',   'ETH-USDT现货',   10, 1, 20180112, 99999999),
       (3, 'LEO-USDT',   'LEO-USDT现货',   10, 1, 20200102, 99999999),
       (3, 'LTC-USDT',   'LTC-USDT现货',   10, 1, 20200103, 99999999),
       (3, 'MATIC-USDT', 'MATIC-USDT现货', 10, 1, 20210331, 99999999),
       (3, 'SHIB-USDT',  'SHIB-USDT现货',  10, 1, 20210510, 99999999),
       (3, 'SOL-USDT',   'SOL-USDT现货',   10, 1, 20201002, 99999999),
       (3, 'TON-USDT',   'TON-USDT现货',   10, 1, 20220430, 99999999),
       (3, 'TRX-USDT',   'TRX-USDT现货',   10, 1, 20200102, 99999999),
       (3, 'WBTC-USDT',  'WBTC-USDT现货',  10, 1, 20200915, 99999999),
       (3, 'XRP-USDT',   'XRP-USDT现货',   10, 1, 20200102, 99999999)
;

UPDATE
    `version`
set `version` = 11;

COMMIT;
