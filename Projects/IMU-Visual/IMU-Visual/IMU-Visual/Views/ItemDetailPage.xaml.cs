using IMU_Visual.ViewModels;
using System.ComponentModel;
using Xamarin.Forms;

namespace IMU_Visual.Views
{
    public partial class ItemDetailPage : ContentPage
    {
        public ItemDetailPage()
        {
            InitializeComponent();
            BindingContext = new ItemDetailViewModel();
        }
    }
}