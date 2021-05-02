$(buttonExit).on('click', e => {
   stopServer();
});

startServer(res => {
   if (res.status == 'Connected')
      loadPage('game');
});

setInterval(() => {
   getTasks(tasks => {
      if (tasks.length > 0)
         console.log('Running tasks:', tasks);
      for (const [task, args] of tasks) {
         window[task](...args);
      }
   })
}, 100);