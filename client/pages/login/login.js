$(buttonRegister).on('click', async e => {
   const formData = new FormData(form);
   const res = await new Promise(res => sendRegister(formData.get('login'), formData.get('password'), res));

   if (res.status == 'OK') {
      loadPage('main-menu');
   } else { // res == 'Exists'
      alert('Аккаунт с таким логином уже существует, придумайте другой логин.');
   }
});

$(buttonLogin).on('click', async e => {
   const formData = new FormData(form);
   const res = await new Promise(res => sendLogin(formData.get('login'), formData.get('password'), res));

   if (res.status == 'OK') {
      loadPage('main-menu');
   } else 
      alert('Вы ввели неверный логин или пароль');
});