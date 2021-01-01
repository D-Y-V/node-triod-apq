const runWorker = require('../build/Release/node-triod-apq.node');

let r = runWorker.run('SELECT * FROM test1 WHERE t = $1 and i = $2', "joe's place", '1');
let r1 =runWorker.run('select * from test1');

r.then(rv => {
    console.log("rv=", rv);
});

r1.then(rv1 => {
    console.log("rv1=", rv1);
});
