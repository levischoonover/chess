const els = {
	board: {
		container: document.querySelector('.board-area-inner'),
		squares: document.querySelector('.squares'),
		pieces: document.querySelector('.pieces')
	}
}


// Board resizing
const resizeBoard = () => {
	const w = els.board.container.parentElement.clientWidth;
	const h = els.board.container.parentElement.clientHeight;
	const newWidth = Math.min(0.9 * w, 0.85 * h);
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


// Chess Pieces
PIECES = {

};