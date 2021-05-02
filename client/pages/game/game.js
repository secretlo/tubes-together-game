$('.cell')
   .hover(
      e => {
         e.target.classList.add('cell_hover');
      },
      e => {
         e.target.classList.remove('cell_hover');
      }
   )
   .click(e => {
      rotateCell(e.target);
   })


Math.randint = (min, max) => (Math.floor(Math.random() * max) + min);

const CELL_TYPES = ['lr', 'tb', 'lrb', 'lrt', 'rtb', 'ltb', 'lrtb'];
document.querySelectorAll('.field .row').forEach((row, i) => {
   row.querySelectorAll('.cell').forEach((cell, j) => {
      setTube(cell, CELL_TYPES[Math.randint(0, 7)]);
      cell.dataset.x = j;
      cell.dataset.y = i;
   });
});
function setTube(cell, type) {
   cell.dataset.type = type;
   cell.style.backgroundImage = `url('${window[`image_tube_${type}`].src}')`;
}

$('.cell_begin').add($('.cell_end')).append($("<div></div>"));


async function rotateCell(cell) {
   if (cell.hasAttribute('is-water')) return;

   const cellType = cell.dataset.type;
   let newType;

   if (cellType == 'tb')
      newType = 'lr';
   else if (cellType == 'lr')
      newType = 'tb';
   else if (cellType == 'lrtb')
      newType = 'lrtb';
   else if (cellType == 'lrb')
      newType = 'ltb';
   else if (cellType == 'ltb')
      newType = 'lrt';
   else if (cellType == 'lrt')
      newType = 'rtb';
   else if (cellType == 'rtb')
      newType = 'lrb';

   const animation = cell.animate([{transform: 'rotate(0deg)'}, {transform: 'rotate(90deg)'}], {easing: 'ease', duration: 500});

   animation.onfinish = () => {
      cell.dataset.type = newType;
      cell.style.backgroundImage = `url('${window[`image_tube_${newType}`].src}')`;
   };
}

$('footer').click(e => {
   $('.cell').each((i, cell) => {
      cell.style.backgroundImage = `url('${window[`image_tube_${cell.dataset.type}`].src}')`;
      cell.removeAttribute('is-water');
   });
});
$('header').click(e => startWater());
async function startWater() {
   const cellBegin = $('.cell_begin')[0];
   await sendWater(cellBegin, null, null);
   alert('Finished');
}
async function sendWater(cell, senderX, senderY) {
   if (cell.hasAttribute('is-water'))
      return;

   const x = +cell.dataset.x, y = +cell.dataset.y,
      type = cell.dataset.type;

   if (senderX == null && senderY == null)
      await fillWater(cell);
   else if (
      senderX < x && type.includes('l') ||
      senderX > x && type.includes('r') ||
      senderY < y && type.includes('t') ||
      senderY > y && type.includes('b')
   )
      await fillWater(cell);
   else
      return;
   
   if (x == 0 || x == 11-1 || y == 0 || y == 7-1) { // is border cell
      if (cell.classList.contains('cell_end'))
         if (x == 0 || x == 11-1) {
            if ( // check cell ends correct (no wall on end)
               x == 0 && type.includes('l') ||
               x == 11-1 && type.includes('r')
            )
               win();
         }
         else { // y == 0 || y == 7-1
            if (
               y == 0 && type.includes('t') ||
               y == 7-1 && type.includes('b')
            )
               win();
         }
   }

   let directions = [];
   const xy = (x, y) => ({x, y});

   if (type.includes('l'))
      directions.push(xy(x-1, y));
   if (type.includes('r'))
      directions.push(xy(x+1, y));
   if (type.includes('t'))
      directions.push(xy(x, y-1));
   if (type.includes('b'))
      directions.push(xy(x, y+1));

   directions = directions.filter(({x, y}) => {
      if (x < 0 || x > 11-1) return false;
      if (y < 0 || y > 7-1) return false;
      return true;
   });
   
   const promises = [];
   for (const direction of directions) {
      const cell = $(`.cell[data-x="${direction.x}"][data-y="${direction.y}"]`)[0];
      promises.push(sendWater(cell, x, y));
   }

   await Promise.all(promises);
}
async function fillWater(cell) {
   await new Promise(res => setTimeout(res, 1000));

   const type = cell.dataset.type;
   cell.style.backgroundImage = `url('${window[`image_water_${type}`].src}')`;
   cell.setAttribute('is-water', '');
}


function win() {
   alert('WIN!!!');
}


setInterval(() => {
   getTasks(tasks => {
      if (tasks.length > 0)
         console.log('Running tasks:', tasks);
      for (const [task, args] of tasks) {
         window[task](...args);
      }
   })
}, 100);