$('.cell')
   .hover(
      e => {
         sendHover(e.target.dataset.x, e.target.dataset.y);
      },
      e => {
         sendUnhover(e.target.dataset.x, e.target.dataset.y);
      }
   )
   .click(e => {
      if (!isMyOrder()) {
         alert('Сейчас не ваша очередь! Подождите пока походит партнер ;)');
         return;
      }

      const cell = e.target,
         x = cell.dataset.x,
         y = cell.dataset.y;
      
      sendRotate(x, y);
   })

function isMyOrder() {
   return !orderMe.classList.contains('dn');
}

function forceHover(x, y, host) {
   const cell = getCell(x, y);
   
   cell.wasForceHover = true;
   cell.classList.add('cell_hover');
}
function forceUnhover(x, y, host) {
   const cell = getCell(x, y);

   cell.wasForceHover = false;
   cell.classList.remove('cell_hover');
}

Math.randint = (min, max) => (Math.floor(Math.random() * max) + min);

function buildField(field) {
   document.querySelectorAll('.field .row').forEach((row, y) => {
      row.querySelectorAll('.cell').forEach((cell, x) => {
         setTube(cell, field[y][x]);
         cell.dataset.x = x;
         cell.dataset.y = y;
      });
   });
}

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
   
   const startScale = cell.classList.contains('cell_hover') ? '1.2' : '1.0';
   const startBrad = cell.classList.contains('cell_hover') ? '10px' : '0px';

   const animation = cell.animate([
      {
         transform: `rotate(0deg) scale(${startScale})`, 
         borderRadius: startBrad,
         zIndex: 2, 
         offset: 0.00
      }, 
      {
         transform: `rotate(0deg) scale(1.5)`,
         borderRadius: '10px',
         zIndex: 2,
         offset: 0.10
      }, 
      {
         transform: `rotate(90deg) scale(1.5)`, 
         borderRadius: '10px',
         zIndex: 2, 
         offset: 0.90
      },
      {
         transform: `rotate(90deg) scale(${startScale})`, 
         borderRadius: startBrad,
         zIndex: 2, 
         offset: 1.00
      },
   ], {easing: 'ease', duration: 500});

   animation.onfinish = () => {
      cell.dataset.type = newType;
      cell.style.backgroundImage = `url('${window[`image_tube_${newType}`].src}')`;
   };
}

function makeStepXY(x, y) {
   const cell = getCell(x, y);
   makeStep(cell);
}
function makeStep(cell) {
   rotateCell(cell);
   
   if (orderMe.classList.contains('dn')) { // was partner turn
      orderMe.classList.remove('dn');
      orderPartner.classList.add('dn');
   } else { // was my turn
      orderMe.classList.add('dn');
      orderPartner.classList.remove('dn');
   }
}

function getCell(x, y) {
   return $(`.cell[data-x="${x}"][data-y="${y}"]`)[0];  
}

//$('footer').click(e => {
//   $('.cell').each((i, cell) => {
//      cell.style.backgroundImage = `url('${window[`image_tube_${cell.dataset.type}`].src}')`;
//      cell.removeAttribute('is-water');
//   });
//});
//$('header').click(e => startWater());

function onWaterFinished() {
   if (!wasWin) // lose
      alert('Вы проиграли..');
   alert('Течение воды завершилось, вы будете перенаправлены на главный экран');
   loadPage('main-menu');
}

let wasWaterStart = false;
async function startWater() {
   if (wasWaterStart) return;
   wasWaterStart = true;

   const cellBegin = $('.cell_begin')[0];

   // check water start is OK
   const x = cellBegin.dataset.x, y = cellBegin.dataset.y,
      type = cellBegin.dataset.type;
   if (x == 0 || x == 11-1) { // left or right edge
      if (!type.includes(x == 0 ? 'l' : 'r')) {
         onWaterFinished();
         return;
      }
   }
   else { // top or bottom edge
      if (!type.includes(y == 0 ? 't' : 'b')) {
         onWaterFinished();
         return;
      }
   }

   await sendWater(cellBegin, null, null);
   onWaterFinished();
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
      const cell = getCell(direction.x, direction.y);
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

let wasWin = false;
function win() {
   if (wasWin) return;
   
   alert('Вы выиграли!!');
   wasWin = true;
}


let HOST;
getUserHost(host => {
   HOST = host;
});
getGameData(gameData => {
   const { session_id, players, field } = gameData;
   buildField(field);

   if (players[0].host == HOST) {
      orderMe.classList.remove('dn');
      orderPartner.classList.add('dn');
   } else {
      orderMe.classList.add('dn');
      orderPartner.classList.remove('dn');
   }
});

let secondsLeft = 10;
setTimeLeft(secondsLeft);
const clockInterval = setInterval(() => {
   setTimeLeft(secondsLeft--);
   if (secondsLeft == -1) {
      clearInterval(clockInterval);
      sendStartWater();
   }
}, 1000);

function setTimeLeft(incomingSeconds) {
   const date = new Date(0, 0, 0, 0, 0, incomingSeconds, 0);
   let minutes = date.getMinutes().toString(),
      seconds = date.getSeconds().toString();
   
   minutes = '0'.repeat(2 - minutes.length) + minutes;
   seconds = '0'.repeat(2 - seconds.length) + seconds;

   timeLeft.textContent = minutes + ':' + seconds;
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