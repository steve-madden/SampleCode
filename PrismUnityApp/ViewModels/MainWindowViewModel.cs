using Prism.Commands;
using Prism.Mvvm;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows.Data;

namespace PrismUnityApp.ViewModels
{
    public class MainWindowViewModel : BindableBase
    {
        private string _title = "Prism Unity Application";
        private Random _rnd = new Random();
        public string Title
        {
            get { return _title; }
            set { SetProperty(ref _title, value); }
        }

        private FastObservableCollection<UserGroupViewModel> _users { get; } = new FastObservableCollection<UserGroupViewModel>();
        private CollectionViewSource _cvs;

        public MainWindowViewModel()
        {
            ReloadCommand = new DelegateCommand(OnReload);
            _cvs = new CollectionViewSource();
            _cvs.Source = _users;
            _cvs.SortDescriptions.Add(new System.ComponentModel.SortDescription("Name", System.ComponentModel.ListSortDirection.Ascending));
        }


        public int NumberOfNames { get; set; } = 100;

        private string _filter;
        public string Filter
        {
            get { return _filter; }
            set { SetProperty(ref _filter, value); }
        }

        public ICollectionView View => _cvs.View;
        public DelegateCommand ReloadCommand { get; set; }

        public void OnReload()
        {
            _users.Clear();
            List<UserGroupViewModel> newUsers = new List<UserGroupViewModel>();
            Random rnd = new Random();
            string[] firstNameRows = System.IO.File.ReadAllLines("Assets/FirstNames.txt");
            int firstNamesCount = firstNameRows.Length;
            string[] lastNameRows = System.IO.File.ReadAllLines("Assets/LastNames.txt");
            int lastNamesCount = lastNameRows.Length;
            for (int i = 0; i < NumberOfNames; i++)
            {
                string lastNameRow = lastNameRows[rnd.Next(lastNamesCount)];
                var lastNameUpper = lastNameRow.Split(' ')[0];
                string lastName = lastNameUpper[0] + lastNameUpper.Substring(1).ToLower();
                string firstNameRow = firstNameRows[rnd.Next(firstNamesCount)];
                string firstNameUpper = firstNameRow.Split(' ')[0];
                string firstName = firstNameUpper[0] + firstNameUpper.Substring(1).ToLower();
                newUsers.Add(new UserGroupViewModel { Name = $"{firstName} {lastName}", IsGroup = false });
            }
            _users.AddRange(newUsers);
            View.Refresh();
        }
    }
}
