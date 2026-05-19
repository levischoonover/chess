const els = {
	board: {
		container: document.getElementById('board'),
		squares: document.getElementById('squares'),
		pieces: document.getElementById('pieces')
	}
}


// Board resizing
const resizeBoard = () => {
	const newWidth = Math.min(els.board.container.parentElement.clientWidth, els.board.container.parentElement.clientHeight);
	els.board.container.style.width = `${newWidth}px`;
};
window.addEventListener('resize', resizeBoard);
resizeBoard();



// Squares setup
for (let row = 0; row < 8; row++) {
	for (let column = 0; column < 8; column++) {
		const newSquare = document.createElement('div');
		newSquare.classList.add((row + column) % 2 == 0 ? 'square-light' : 'square-dark');
		els.board.squares.appendChild(newSquare);
	}
}
