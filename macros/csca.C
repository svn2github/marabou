// clear scaler
{
	e->ClearCnaf();
	e->SetNofCnafs(10);
	e->SetCnaf(0, crate1, station1, 15, 9);
	e->SetCnaf(1, crate1, station1, 2, 2);
	e->SetCnaf(2, crate1, station1, 3, 2);
	nCnafs = 3;
	if (s2) {
		e->SetCnaf(3, crate2, station2, 15, 9);
		e->SetCnaf(4, crate2, station2, 2, 2);
		e->SetCnaf(5, crate2, station2, 3, 2);
		nCnafs = 6;
	}
	if (s3) {
		e->SetCnaf(6, crate2, station2, 15, 9);
		e->SetCnaf(7, crate2, station2, 2, 2);
		e->SetCnaf(8, crate2, station2, 3, 2);
		nCnafs = 9;
	}
	e->ExecCnafList(nCnafs);

	cout << "ccsca: Scaler " << s1->GetName() << " in " << s1->GetPosition() << ": CLEARED " << endl;
	if (s2) cout << "csca: Scaler " << s2->GetName() << " in " << s2->GetPosition() << ": CLEARED " << endl;
	if (s3) cout << "csca: Scaler " << s3->GetName() << " in " << s3->GetPosition() << ": CLEARED " << endl;
}
