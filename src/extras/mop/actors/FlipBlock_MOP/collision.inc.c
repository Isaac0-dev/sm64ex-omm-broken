const Collision col_FlipBlock_MOP_0x7d1a98[] = {
	COL_INIT(),
	COL_VERTEX_INIT(8),
	COL_VERTEX(106, 104, 106),
	COL_VERTEX(-106, 104, -106),
	COL_VERTEX(-106, 104, 106),
	COL_VERTEX(106, 104, -106),
	COL_VERTEX(106, -104, -106),
	COL_VERTEX(106, -104, 106),
	COL_VERTEX(-106, -104, 106),
	COL_VERTEX(-106, -104, -106),
	COL_TRI_INIT(SURFACE_NOT_SLIPPERY, 12),
	COL_TRI(0, 1, 2),
	COL_TRI(1, 0, 3),
	COL_TRI(0, 4, 3),
	COL_TRI(4, 0, 5),
	COL_TRI(0, 6, 5),
	COL_TRI(6, 0, 2),
	COL_TRI(2, 7, 6),
	COL_TRI(7, 2, 1),
	COL_TRI(1, 4, 7),
	COL_TRI(4, 1, 3),
	COL_TRI(4, 6, 7),
	COL_TRI(6, 4, 5),
	COL_TRI_STOP(),
	COL_END()
};
