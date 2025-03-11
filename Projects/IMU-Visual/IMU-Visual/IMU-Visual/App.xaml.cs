using IMU_Visual.Services;
using IMU_Visual.Views;
using System;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace IMU_Visual
{
    public partial class App : Application
    {

        public App()
        {
            InitializeComponent();

            DependencyService.Register<MockDataStore>();
            MainPage = new AppShell();
        }

        protected override void OnStart()
        {
        }

        protected override void OnSleep()
        {
        }

        protected override void OnResume()
        {
        }
    }
}
