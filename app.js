const http = require('http');
const fs = require('fs');

const server = http.createServer((req, res) => {
	const files = {
		'/': {
			path: 'web/client.html',
			type: 'text/html'
		},
		'/script.js': {
			path: 'web/script.js',
			type: 'text/javascript'
		},
		'/styles.css': {
			path: 'web/styles.css',
			type: 'text/css'
		}
	}
	const file = files[req.url];
	if (file) {
		fs.readFile(file.path, (err, data) => {
			if (err) {
				res.writeHead(500, { 'Content-Type': 'text/plain' });
				res.end('Internal Server Error');
			} else {
				res.writeHead(200, { 'Content-Type': file.type });
				res.end(data, 'utf-8');
			}
		});
	} else {
		res.writeHead(404, { 'Content-Type': 'text/plain' });
		res.end('File not found');
	}
});

const PORT = 8000;

server.listen(PORT, () => {
	console.log('Server is running on http://localhost:' + PORT);
});