using Prism.Mvvm;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace ComboBoxTest
{
    public class MainWindowViewModel : BindableBase
    {
        public ObservableCollection<LookupComboBoxItem> Items { get; } = new ObservableCollection<LookupComboBoxItem>();
        private CollectionViewSource _itemsViewSource { get; } = new CollectionViewSource();

        public MainWindowViewModel()
        {
            var rnd = new Random();
            for( int i = 0; i < 100; i++)
            {
                var lookupItem = new LookupItem
                {
                    Name = string.Format("{0,15}", rnd.Next())
                };
                Items.Add(new LookupComboBoxItem(lookupItem));
            }
            _itemsViewSource.SortDescriptions.Add(new System.ComponentModel.SortDescription("Name", System.ComponentModel.ListSortDirection.Ascending));
            _itemsViewSource.Source = Items;
            SelectedIndex = 0;
        }

        private LookupComboBoxItem _selectedItem;
        public LookupComboBoxItem SelectedItem
        {
            get { return _selectedItem; }
            set { SetProperty(ref _selectedItem, value); }
        }

        public ICollectionView ItemsView { get { return _itemsViewSource.View; } }

        public int SelectedIndex { get; set; }

    }
}
