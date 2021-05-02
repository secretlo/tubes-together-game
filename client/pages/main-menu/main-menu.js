$(buttonExit).on('click', e => {
   loadPage('login');
});
$(buttonStart).on('click', e => {
   loadPage('wait');
});

getLevels(levels => {
   levelsFrom.textContent = levels;
});